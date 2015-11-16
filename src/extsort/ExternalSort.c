#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "ExternalSort.h"
#include "ExternalSortAPI.h"
#ifdef VERBOSE_DEBUG
#define DEBUG_VERBOSE 1
#endif
/*TODO: IN ALL THIS CODE THE RUN-LENGTH IS IN BYTES, HOWEVER
 *IT WOULD BE REALLY GOOD IF WE AVOID STORING ('#OF KEYS'  * KEY_SIZE)
 *RATHER STORE ONLY '#OF KEYS'
 *
 */
/*M is the hypothetical RAM, ensure that the size of buffer for 
 * MBuffer + MWriteBuffer will not overflow
 **/
static void *MBuffer=NULL;
static char *MWriteBuffer=NULL;

inline size_t GetMin(size_t a,size_t b){
	return (a < b)?a:b;
}
FILE *run_file = NULL;

int GetRunFile(){
	if(!run_file){
		run_file = fopen("run_file.bin","r+");
		if(!run_file){
			run_file = fopen("run_file.bin","w+");
		}
		assert(run_file);
	}
	return fileno(run_file);
}
ssize_t SafeWrite(int fd,void *buf,size_t wlen){
	ssize_t len; char *bbuf = (char *)buf;
	size_t writeln=0;
	while(writeln < wlen){
		len = write(fd,&(bbuf[writeln]),wlen-writeln);
		if(len<=0) return len;
		writeln += len;
	}
	return writeln;
}

ssize_t SafeRead(int fd,void *buf,size_t rlen){
	ssize_t len;char *bbuf = (char *)buf;
	size_t readln=0;
	while(readln < rlen){
		len = read(fd,&(bbuf[readln]),(size_t)rlen-readln);
		if(len<=0){
			if(len <0){
				perror("READ FAILED:");
				return len;
			}
			break; /*May be encountered EOF*/
		}
		readln += len;
	}
	return readln;
}

/*A run is written as follows, assumes you know key size 
 * <----UNSIGNED LONG (rlen)----->0,1,2,3.......rlen-1
 */
unsigned long WriteRun(void *buf,int wfd,unsigned long rlen){
	int len;
	unsigned long writeln=0;
	len = write(wfd,(void *)&rlen,sizeof(unsigned long));
	assert(len == sizeof(unsigned long));
	while(writeln < rlen){
		len = write(wfd,(void *)(buf+writeln),(rlen-writeln));
		assert(len>0);
		writeln += len;
	}
	return writeln;
}

void UnlinkRunFile(){ /*Delete the existing run-file*/
	unlink("run_file.bin");
}

FILE *dirty_file=NULL;
int GetDirtyFile(){
	static int dmid;
	if(!dirty_file){
		dirty_file = fopen("dirty_sequence_file.txt","w");
		assert(dirty_file);
		dmid = fileno(dirty_file);
	}
	return dmid;
}
/*Takes in a binary-run file and verifies it*/
unsigned char VerifyRunFile(FILE *runfile, 
	ExKeyCompare key_compare, unsigned int KEY_SIZE){
	unsigned long runlen;
	unsigned long klen;
	unsigned long i;
	unsigned char a[KEY_SIZE], b[KEY_SIZE];
	unsigned char *curr, *prev;
	ssize_t rout;
	unsigned long rcount = 0;

	while((rout = fread(&runlen, sizeof(unsigned long), 
			1, runfile)) > 0){
		klen = runlen/KEY_SIZE;
		printf("Verifying RUN-%lu of %lu keys", 
			++rcount, klen);
		if(klen){
			rout = fread(a, KEY_SIZE, 1, runfile);
			assert(rout > 0); prev = a; curr = a;
			for(i=1; i<klen; i++){
				curr = (i&1)?b:a;
				rout = fread(curr, KEY_SIZE, 1, runfile);
				assert(rout > 0);
				if(!key_compare(prev, curr)){
					printf("[FAILED]\n");
					return 0;
				}
				prev = curr;
			}
		}
		printf("[SUCCESS]\n");
	}
	fclose(runfile);
	return 1;
}

void CreateMBuffer(unsigned int R, unsigned int BLOCK_SIZE){
	if(!MBuffer){
		/* C*D*BLOCK_SIZE*K */
		MBuffer = malloc((size_t)((R*C_SIZE_MIN*BLOCK_SIZE)));
		/* Also create MWriteBuffer*/
		MWriteBuffer = malloc((size_t)(R*C_SIZE_MIN*BLOCK_SIZE));
	}
	assert(MBuffer && MWriteBuffer);
}
/*Default Pass File*/
static char PASS_FILE_NAME[32];
int GetPassFile(){
	int ret_mkstemp = -1;

	strcpy(PASS_FILE_NAME, "ex-sort-passXXXXXXX");
	ret_mkstemp = mkstemp(PASS_FILE_NAME);
	assert(ret_mkstemp != EINVAL && ret_mkstemp != EEXIST);
	return ret_mkstemp;
}

void TopLevelRWayMerge(const char *,  unsigned int, 
	unsigned int KEY_SIZE, ExKeyCompare, unsigned long rcount);

/*R is number of runs to be merged in a single pass*/
void SetupMergeBuffers(unsigned int R,unsigned int C, 
	unsigned int BLOCK_SIZE, unsigned int KEY_SIZE){
	if(!MWriteBuffer){
		MWriteBuffer = malloc(R*C*BLOCK_SIZE*KEY_SIZE);
		assert(MWriteBuffer);
	}
}
inline char CheckMWriteBufferInt(int *buf,unsigned int size){
#ifdef CHECK_MBUFFER
	unsigned int i;
	for(i=1;i<size;i++){
		assert(buf[i-1] <= buf[i]);
	}
#endif
	return 0;
}
/*'seek_start' is the point in the file from where you start the 
 * binary search
 */
unsigned char ExternalBinarySearch(FILE *sorted_key_file, unsigned long keylen,
	unsigned char *key, ExKeyBinCompare key_compare, unsigned char KEY_SIZE, 
	unsigned char *answer, off_t seek_start){
	unsigned long beg, end;
	off_t seek_ptr; void *daddr;
	unsigned char rbuffer[KEY_SIZE];
	unsigned char ret_comp; size_t ret_read;
	int ret_seek; unsigned char found;


	beg = 1; end = keylen; found = 0;
	while(beg < end){
		
		seek_ptr = (off_t) ((((beg+end)/2)-1)*KEY_SIZE);
		ret_seek = fseek(sorted_key_file, seek_ptr+seek_start, SEEK_SET);
		assert(!ret_seek);
		
		ret_read = fread(rbuffer, KEY_SIZE, 1, sorted_key_file);
		assert(ret_read == 1);

		ret_comp = key_compare(key, rbuffer);
		if(!ret_comp){
			beg = ((beg+end)/2)+1;
		}else if(ret_comp&1){
			end = ((beg+end)/2)-1;
		}else{
			found = 1;
			break;
		}
	}

	if(!found && end){ /*Check the last key*/
		seek_ptr = (off_t) (end-1)*KEY_SIZE;
		ret_seek = fseek(sorted_key_file, seek_ptr+seek_start, SEEK_SET);
		assert(!ret_seek);
		
		ret_read = fread(rbuffer, KEY_SIZE, 1, sorted_key_file);
		assert(ret_read == 1);

		ret_comp = key_compare(key, rbuffer);
		found = (ret_comp == 2)?1:0;
	}
	/*the value in the 'end' is the largest number smaller 
	 *than the give key
	 */
	if(found){
		daddr = memcpy(answer, rbuffer, KEY_SIZE);
		assert(daddr == answer);
	}
	return found;
}
void ExternalRWayMerge(const char *file_name, unsigned int R, 
	unsigned int KEY_SIZE,ExKeyCompare keycompare, unsigned long rcount){
	TopLevelRWayMerge(file_name, R, KEY_SIZE, keycompare, rcount);
}
/*We have to perform log(N/M)/log(R) passes through the data.*/
inline void TopLevelRWayMerge(const char *run_file_name, unsigned int R, 
	unsigned int KEY_SIZE, ExKeyCompare KeyCompare, unsigned long runcount){

	unsigned long runs = runcount;
	unsigned long merged_runs = 0;
	unsigned long *runlens = NULL;
	unsigned int finished_runs=0;
	unsigned int j,k,run_index = 0;
	unsigned long windex;
	size_t readln=0;char stop_merge;
	off_t *run_offs = NULL, ret_off;
	unsigned long *runheads = NULL;
	unsigned int *merge_index = NULL;
	unsigned int *merge_index_max = NULL;
	char **merge_ptr=NULL;
	char **merge_ptr_start=NULL;
	char *cbuf ; char *cmd_buffer; 
	int len; char min_picked=0; void *current_min = NULL;
	unsigned long good_length=0; off_t run_start_off = 0,curr_seek_off;
	unsigned long total_runlens=0;
	char merge_just_started=0;
	off_t seek_start=0;
	unsigned int BLOCK_SIZE;
	void *daddr = NULL; ssize_t wlen, ret_ssize;
	unsigned int MERGE_R = R;
	FILE *run_file_ptr = fopen(run_file_name,"r+");
	int passfd = GetPassFile(); 
	int runfd = fileno(run_file_ptr);
	int original_runfd = runfd, ret_val;
	assert(runfd > 0 && run_file_ptr && passfd > 0);
	

	/* NOTE: ALWAYS MAKE SURE THAT THE BLOCK_SIZE/KEY_SIZE 
 	 * is an integer
	 */
	BLOCK_SIZE = (OS_PAGE_SIZE)*KEY_SIZE;
	/*FRIST SET-UP THE MERGE BUFFER*/
	CreateMBuffer(R, BLOCK_SIZE);
	cbuf = (char *)MBuffer;
	/*Sanity Check*/
	runlens = (unsigned long *) malloc(sizeof(unsigned long)*R);
	run_offs  = (off_t *) malloc(sizeof(off_t)*R);
	runheads = (unsigned long *) malloc(sizeof(unsigned long)*R);
	merge_index = (unsigned int *) malloc(sizeof(unsigned int)*R);
	merge_index_max = (unsigned int *)malloc(sizeof(unsigned int)*R);
	merge_ptr = (char **) malloc(sizeof(char*)*R);
	merge_ptr_start = (char **) malloc(sizeof(char*)*R);
	assert(runlens); assert(merge_index_max);
	assert(run_offs);assert(merge_ptr);
	assert(runheads);assert(merge_index);

	ret_off = lseek(runfd,(off_t)0,SEEK_SET); assert(ret_off != (off_t)-1);
	ret_off = lseek(passfd,(off_t)0,SEEK_SET); assert(ret_off != (off_t)-1);

	while(runs>1){
		merged_runs = 0;
		for(j=0;j<runs;j+=MERGE_R){
			/*STEP1: Read the lengths of the runs, set the run heads*/
			seek_start = lseek(runfd,(off_t)0,SEEK_CUR);
			total_runlens=0;
			R = ((runs-j) < MERGE_R)?(runs-j):MERGE_R;
			for(k=0;k<R;k++){ 
				len = read(runfd,&(runlens[k]),sizeof(unsigned long));
				runlens[k] = runlens[k]/KEY_SIZE;
				total_runlens += runlens[k];
				assert(len >= 0);
				runheads[k] = 0;
#if DEBUG_VERBOSE  
				fprintf(stdout,"runid=%d runlen=%lu \n",k,runlens[k]);
				fflush(stdout);
#endif
				if(!len) break;
				run_offs[k] = lseek(runfd, (off_t)0, SEEK_CUR);
				lseek(runfd, (off_t)(runlens[k]*KEY_SIZE), SEEK_CUR);
			}
			/*STEP2: Merge 'R' runs into 1 run 
			 *(taking dirty sequence out: NOTE this 
			 * version has no dirty sequence)
			 */
			stop_merge = 0; windex = 0;
			finished_runs = 0;
			merge_just_started=1;
			good_length = 0;

			while(finished_runs < R){
				for(k=0;k<R;k++){
					if(runheads[k] == runlens[k]){
						/*a run has length of atleast 1*/
						merge_index_max[k] = 0;
						finished_runs++;
						continue;
					}
					lseek(runfd, run_offs[k], SEEK_SET);
					readln = GetMin((C_SIZE_MIN*BLOCK_SIZE),
						(runlens[k]-runheads[k])*KEY_SIZE);
#if DEBUG_VERBOSE
					if(readln < C_SIZE_MIN*BLOCK_SIZE){
						printf("Reading Tail part of Run %d \n",k);
					}
#endif
					len = SafeRead(runfd, 
						&(cbuf[k*C_SIZE_MIN*BLOCK_SIZE]), readln);
					if(len < 0){
						perror("READ:FAILED:");
					}
					merge_ptr_start[k] = 
						(char *)&(cbuf[k*C_SIZE_MIN*BLOCK_SIZE]);
					merge_ptr[k] = merge_ptr_start[k];
					run_offs[k] = lseek(runfd,(off_t)0,SEEK_CUR);
					merge_index_max[k] = readln/KEY_SIZE;
					assert(merge_index_max[k] <= runlens[k]);
					
					/*Move the head by what ever is read*/
					runheads[k] += merge_index_max[k];
					if(runheads[k] == runlens[k]){
						assert(finished_runs != R);
						finished_runs++;
					}
				}
				/*The Merge length of each run is min(C_SIZE*BLOCK_SIZE, 
				 *(runlens[k]-runheads[k]+1)
				 */
				/*STEP4: Do a R-Way merge Setup the merge indexes*/
				for(k=0;k<R;k++){
					merge_index[k] = 0;
				}
				windex = 0;
				stop_merge=0;
				
				while(!stop_merge){
					min_picked=0;
					for(k=0;k<R;k++){
						if(merge_index[k] == merge_index_max[k]){
							/*Skip this run it has expired*/
							continue;
						}

						if(!min_picked){
							current_min = merge_ptr[k];
							min_picked=1;
						}
						if(KeyCompare(merge_ptr[k],current_min)){
							current_min = merge_ptr[k];
							run_index = k;
						}
					}

					if(!min_picked){
						assert(finished_runs == R);
						if(windex > 0){
							if(merge_just_started){
								windex = windex*KEY_SIZE;
								run_start_off = lseek(passfd, (off_t)0, SEEK_CUR);
								assert(run_start_off != (off_t) -1);
								ret_ssize = write(passfd, (void *)&windex, 
									sizeof(unsigned long));
								assert(ret_ssize == sizeof(unsigned long));
								windex = windex/KEY_SIZE;
								merge_just_started=0;
							}
							ret_ssize = SafeWrite(passfd,(void *) MWriteBuffer,
								windex*KEY_SIZE);
							assert(ret_ssize == (windex*KEY_SIZE));
							windex=0;
						}
						/*stop_merge=1; continue;*/
						break;
					}

					/*Found a minimum key*/
					daddr = memcpy((MWriteBuffer+windex*KEY_SIZE), 
						merge_ptr[run_index], KEY_SIZE); 
					assert(daddr == (MWriteBuffer+windex*KEY_SIZE));

#if DEBUG_VERBOSE
					if(windex >=1){
						if(!KeyCompare(MWriteBuffer+(windex-1)*KEY_SIZE, 
								MWriteBuffer+KEY_SIZE*windex)){ 
							/*This should never happen*/
							printf("windex = %lu %d %d\n", windex, 
								MWriteBuffer[(windex-1)*KEY_SIZE], 
								MWriteBuffer[windex*KEY_SIZE]);
							printf("The run=%u , runlens=%lu ,runheads=%lu, merge_index=%u , merge_imax = %u\n",
								run_index, runlens[run_index], runheads[run_index], merge_index[k],
								merge_index_max[k]);
							assert(0);
						}
					}
#endif
					windex++; good_length++;
					merge_index[run_index]++;
					merge_ptr[run_index] += KEY_SIZE;

					/**********ACTIONS**********
					 * 1. If DB keys are filled (refill the buffers)
					 * 2. If keys in the blocks fetched for a run finished
					 *    (do a cleanup exclude the dirty sequence).
					 * 3. A run has reached its end (continue merging)
					 */
					if(merge_index[run_index] == merge_index_max[run_index]){
						if(runheads[run_index] == runlens[run_index]){
							continue; /*continue merging*/
						}
						if(merge_just_started){
							windex = windex*KEY_SIZE;
							run_start_off = lseek(passfd, (off_t)0,SEEK_CUR);
							wlen = write(passfd, 
								(void *)&windex, sizeof(unsigned long));
							assert(wlen == sizeof(unsigned long));
							windex = windex/KEY_SIZE;
							merge_just_started=0;
						}
						wlen = SafeWrite(passfd, (void *)MWriteBuffer, 
							windex*KEY_SIZE);
						assert(wlen == (windex*KEY_SIZE));
						/*This is the simple R-Way merge, refill this block*/
						readln = GetMin((runlens[run_index]-runheads[run_index]),
							((C_SIZE_MIN*BLOCK_SIZE)/KEY_SIZE));

						ret_off = lseek(runfd,run_offs[run_index],SEEK_SET);
						assert(ret_off != (off_t)-1);
						ret_ssize = SafeRead(runfd,merge_ptr_start[run_index],
							readln*KEY_SIZE);
						assert(ret_ssize == (readln*KEY_SIZE));
						run_offs[run_index] = lseek(runfd,(off_t)0,SEEK_CUR); 
						assert(run_offs[run_index] != (off_t)-1);
						runheads[run_index] += readln;  /*[CAREFUL] the readln here is already divded by KEYSIZE*/
						if(runheads[run_index] == runlens[run_index]){
							assert(finished_runs != R);
							finished_runs++;
						}
						merge_ptr[run_index] = merge_ptr_start[run_index];
						merge_index[run_index] = 0;
						merge_index_max[run_index] = readln;
						assert(merge_index_max[run_index] < runlens[run_index]);
						assert(merge_index_max[run_index] <= ((C_SIZE_MIN*BLOCK_SIZE)/KEY_SIZE));
						
						windex = 0;
						continue; /*continue merging*/
					}
				}
			}
			assert(windex == 0);
			
			/*Update the runsize in the passfile*/	
			curr_seek_off = lseek(passfd,(off_t)0,SEEK_CUR);
			assert(curr_seek_off != (off_t)-1);
			ret_off = lseek(passfd,run_start_off,SEEK_SET);
			assert(ret_off !=(off_t)-1);
			good_length *= KEY_SIZE;
			ret_ssize = write(passfd,&(good_length),sizeof(unsigned long));
			assert(ret_ssize == sizeof(unsigned long));
			good_length /= KEY_SIZE;
			ret_off = lseek(passfd,(off_t)curr_seek_off,SEEK_SET);
			assert(ret_off !=(off_t) -1);
			/************<Done Updating>*******************/

			/*Seek the run file so that it can read next runs
			 *(KEY_SIZE*(good_length+dirty_length)+R*sizeof(unsigned long)
			 */
			ret_off = lseek(runfd,(off_t)(seek_start+KEY_SIZE*(good_length)+
				R*(sizeof(unsigned long))),SEEK_SET);
			assert(ret_off != (off_t) -1);
			merged_runs++;
		}
		runs = merged_runs;
		/*Exchange passfd and runfd*/
		passfd = passfd^runfd;
		runfd =  passfd^runfd;
		passfd = passfd^runfd;
		ret_off = lseek(runfd,(off_t)0,SEEK_SET);
		assert(ret_off != (off_t)-1);
		ret_off = lseek(passfd,(off_t)0,SEEK_SET); 
		assert(ret_off != (off_t)-1);
		ret_val = ftruncate(passfd,0);
		assert(!ret_val);
	}

	runlens[0] = 0;
	len = read(runfd, &(runlens[0]), sizeof(unsigned long));
	runlens[0] /= KEY_SIZE;
	fprintf(stdout,"[FINAL-RUN] has %lu keys\n", runlens[0]);
	close(runfd); close(passfd);

	if(original_runfd != runfd){
		unlink(run_file_name);
		ret_val = rename(PASS_FILE_NAME, run_file_name);
		if(ret_val < 0){
			perror("TopLevelRwayMerge:");
			fprintf(stderr, "Renaming Failed: pass_file=%s run_file_name=%s\n", PASS_FILE_NAME, run_file_name);
			fprintf(stdout,"%80s", "TRYING TO RENAME FROM SHELL\n");
			cmd_buffer = (char *) malloc(sizeof(char)*(strlen(PASS_FILE_NAME)+strlen(run_file_name)+128));
			assert(cmd_buffer);
			sprintf(cmd_buffer, "mv %s %s", PASS_FILE_NAME, run_file_name);
			ret_val = system(cmd_buffer);
			if(ret_val < 0){
				fprintf(stdout, "%80s", "SHELL RENAME FAILED\n");
			}else{
				fprintf(stdout, "%80s", "SHELL RENAME SUCCESS\n");
			}
			free(cmd_buffer);
		}
	}else{ /*remove the pass-file*/
		ret_val = remove(PASS_FILE_NAME);
		if(ret_val){
			perror("TopLevelRwayMerge:");
		}
	}
	free(MBuffer); free(MWriteBuffer);
	free(runlens); free(run_offs);
	free(runheads); free(merge_index);
	free(merge_index_max); free(merge_ptr);
	free(merge_ptr_start); 
	MBuffer = NULL; MWriteBuffer = NULL;
}


/*Does the final scan and */
#define SCAN_SIZE 1024*4096
void ConvertRunToSequence(const char *run_file, 
	const char *out_file){
	FILE *in_fptr = fopen(run_file, "r");
	FILE *out_fptr = fopen(out_file, "w");
	unsigned long dummy;
	size_t ret_read, ret_write;
	unsigned char *buffer = NULL;

	buffer = (unsigned char *) malloc(sizeof(unsigned char)*SCAN_SIZE);
	assert(buffer);

	printf("Converting the runfile=%s to output=%s\n", 
		run_file, out_file);
	assert(in_fptr && out_fptr);
	ret_read = fread(&dummy, sizeof(unsigned long), 1, in_fptr);
	assert(ret_read == 1);
	
	while((ret_read = fread(buffer, sizeof(unsigned char), SCAN_SIZE, 
			in_fptr))){
		ret_write = fwrite(buffer, sizeof(unsigned char), ret_read, 
			out_fptr);
		assert(ret_write == ret_read);
	}
	free(buffer);
	fclose(in_fptr);
	fclose(out_fptr);
}

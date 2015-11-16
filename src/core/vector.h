/**
 * Posix compliant version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2015 NEngine Team
 * 
 * This file is part of CSA-Rainbow-Table-Tool.
 * 
 * CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSA-Rainbow-Table-Tool.
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 * This software is based on the windows version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2012  Colibri <colibri.dvb@googlemail.com>  
 * http://colibri.net63.net/ alias http://colibri-dvb.info/
 * 
 */
#if !defined(PCRTT_SRC_CORE_VECTOR_HEADER_H_)
#define PCRTT_SRC_CORE_VECTOR_HEADER_H_

#include <algorithm>

namespace PCRTT {

template <class Type> class Vector
{
public:
	// Vector::Iterator
	class Iterator
	{			
		protected:
			friend class Vector;
			Iterator(Type *ptr) : ptr(ptr) { }
		public:
			Iterator() : ptr(0) { }
			Iterator(const Iterator &it) : ptr(it.ptr) { }
			virtual ~Iterator() { }

			Iterator &operator=(const Iterator &it)
			{ ptr = it.ptr; return *this; }

			int operator==(const Iterator &it) const
			{ return ptr == it.ptr; }
			int operator!=(const Iterator &it) const
			{ return ptr != it.ptr; }

			Type &operator*() { return *ptr; }
			const Type &operator*() const
			{ return *ptr; }

			Type *operator->() { return ptr; }

			const Type *operator->() const { return ptr; }

			Iterator &operator++() { ++ptr; return *this; }
			Iterator operator++(int) {
				Iterator tmp = *this; ++ptr; return tmp;
			}

			Iterator operator+(int n) const { return Iterator(ptr + n); }
			Iterator operator-(int n) const { return Iterator(ptr - n); }
			Iterator &operator+=(int n) { ptr += n; return *this; }
			Iterator &operator-=(int n) { ptr -= n; return *this; }

			int operator-(const Iterator &it) const {
				return (int)(ptr - it.ptr);
			}
		protected:
			Type *ptr;
	}; // end of class Iterator
		
	// Vector
	Vector() : length(0), capacity(0), data(0) { }
	Vector(const Vector &v) : length(v.length), capacity(v.length), data(0)
	{
		data = new Type[capacity];
		for(int i = 0; i < length; ++i)
		{
			data[i] = v.data[i];
		}
	}
	Vector(int size) : length(0), capacity(size), data(0)
	{
		data = new Type[capacity];
	}
	Vector(Type *v,int size) : length(size), capacity(size), data(0)
	{
		data = new Type[capacity];
		for(int i = 0; i < length; ++i)
		{
			data[i] = v[i];
		}
	}

	virtual ~Vector()
	{
		KILLARRAY(data);
	}
		
	Vector &operator=(const Vector &v)
	{
		if(v.length > capacity)
		{
			capacity = v.length;
			Type *new_data = new Type[capacity];
			KILLARRAY(data);
			data = new_data;
		}
		length = v.length;
		for(int i = 0; i < length; ++i)
		{
			data[i] = v.data[i];
		}
		return *this;
	}

	inline Type &operator[](int index)
	{
		return data[index];
	}

	inline const Type &operator[](int index) const
	{
		return data[index]; 
	}

	// cast to pointer
	inline operator void*()
	{
		return &data[0];
	}

	inline operator const void*()
	{
		return &data[0];
	}

	inline Iterator begin() const
	{
		return Iterator(data);
	}

	inline Iterator end() const
	{
		return Iterator(data + length);
	}

	inline int size() const
	{
		return length;
	}

	void resize(int size)
	{
		if(size <= capacity)
		{
			length = size;
			return;
		}
		capacity = size;
		Type *new_data = new Type[capacity];
		for(int i = 0; i < length; ++i)
		{
			new_data[i] = data[i];
		}
		KILLARRAY(data);
		data = new_data;
		length = size;
	}

	void reserve(int size)
	{
		if(size <= capacity)
		{
			return;
		}
		capacity = size;
		Type *new_data = new Type[capacity];
		for(int i = 0; i < length; ++i)
		{
			new_data[i] = data[i];
		}
		KILLARRAY(data);
		data = new_data;
	}

	void clear()
	{
		length = 0;
	}

	Iterator find(const Type &t)
	{
		for(int i = 0; i < length; ++i)
		{
			if(data[i] == t) return Iterator(data + i);
		}
		return Iterator(data + length);
	}

	void append(const Type &t)
	{
		if(length + 1 <= capacity)
		{
			data[length++] = t;
			return;
		}
		int old_length = length;
		resize(length * 2 + 1);
		length = old_length + 1;
		data[old_length] = t;
	}

	void append(int position,const Type &t)
	{
		if(length + 1 <= capacity)
		{
			for(int i = length - 1; i >= position; i--)
			{
				data[i + 1] = data[i];
			}
			data[position] = t;
			++length;
			return;
		}
		capacity = length * 2 + 1;
		Type *new_data = new Type[capacity];
		for(int i = 0; i < position; ++i)
		{
			new_data[i] = data[i];
		}
		new_data[position] = t;
		for(int i = position; i < length; ++i)
		{
			new_data[i + 1] = data[i];
		}
		KILLARRAY(data);
		data = new_data;
		++length;
	}

	void append(const Iterator &it,const Type &t)
	{
		append(it - begin(),t);
	}

	void remove()
	{
		length--;
	}

	void remove(int position)
	{
		if ( length <= 0 )
			return;
		for(int i = position; i < length - 1; ++i)
		{
			data[i] = data[i + 1];
		}
		length--;
		if ( length <= 0 )
			length = 0;
	}
	void erase(int position){remove(position);}
	void remove(const Iterator &it) { remove(it - begin()); }
	void erase(const Iterator &it) { remove(it - begin()); }

	void destroy(const Type &t)
	{
		Iterator it = find( t );
		remove( it );
	}

	int addUniq(const Type &t)
	{
		for(int i = 0; i < length; ++i)
		{
			if ( t == data[i] ) return 0; // already exist
		}
		append(t);
		return 1;
	}

	int exist(const Type &t)
	{
		int count = 0;
		for(int i = 0; i < length; ++i)
		{
			if ( t == data[i] ) ++count;
		}
		return count;
	}

	int isEqual(const int& id,const Type &t) const
	{
		if(id>length) return -1;
		return ( t == data[id] );
	}

	// std vector compliant

	// append
	inline void push_back(const Type &t) { append(t); }

	// return last element of mutable sequence
	Type& back()
	{
		if(!size()) resize(1);
		return (*(end() - 1));
	}

	// erase element at end
	void pop_back() { resize(size() - 1); }

#if 0 // todo
	/** used for quicksort algorithm */
	int partition(const int left, const int right)
	{
		const int mid = left + (right - left) / 2;
		const int pivot = data[mid];

		// move the mid point value to the front.
		std::swap( data[mid], data[left] );
		int i = left + 1;
		int j = right;
		while (i <= j) {
			while(i <= j && data[i] <= pivot) {
				i++;
			}

			while(i <= j && data[j] > pivot) {
				j--;
			}

			if (i < j) {
				std::swap(data[i], data[j]);
			}
		}
		std::swap(data[i - 1],data[left]);
		return i - 1;
	}

	/** simple quicksort algorithm */
	inline void quickSort(int left, int right)
	{
		if (left >= right) {
			return;
		}

		int part = partition(left, right);

		quickSort(left, part - 1);
		quickSort(part + 1, right);
	}

	/** use quicksort */
	inline void sort()
	{
		if ( !size() )
			return;
		quickSort(0, length-1);
	}
#endif // todo

private:
	int length, capacity;
	Type *data;

}; // end of class Vector

/** predefined type */
typedef Vector<void*> void_vp; // CPtrList
typedef Vector<uchar*> uchar_vp; // CPtrList

} // end of namespace PCRTT

#endif // PCRTT_SRC_CORE_VECTOR_HEADER_H_

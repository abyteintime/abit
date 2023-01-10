#pragma once

#include <cstdint>
#include <utility>

#include "abit/ue/memory.hpp"

namespace ue {

template<typename Array, typename Element>
struct ArrayRange
{
	const Array* view;

	struct Cursor
	{
		const Element* ptr;

		inline const Element& operator*() const { return *ptr; }

		inline Cursor& operator++()
		{
			++ptr;
			return *this;
		}

		inline bool operator!=(Cursor other) { return ptr != other.ptr; }
	};

	inline Cursor begin() const { return { view->dataPtr }; }
	inline Cursor end() const { return { view->dataPtr + view->length }; }
};

/// Used for view into an existing `TArray<T>`, with the ability to modify it, but without the
/// ability to copy it or move it elsewhere.
template<typename T>
struct ViewIntoTArray
{
	T* dataPtr = nullptr;
	int32_t length = 0;
	int32_t capacity = 0;

	ViewIntoTArray(const ViewIntoTArray<T>&) = delete;
	ViewIntoTArray(ViewIntoTArray<T>&&) = delete;

	const T& operator[](size_t i) const { return dataPtr[i]; }
	T& operator[](size_t i) { return dataPtr[i]; }

	void Clear() { length = 0; }

	void Append(T&& element)
	{
		if (length + 1 > capacity) {
			capacity *= 2;
			dataPtr = reinterpret_cast<T*>(appRealloc(dataPtr, capacity));
		}
		dataPtr[length] = std::move(element);
		length += 1;
	}

	template<typename Range>
	void ExtendByCopying(const Range& range)
	{
		for (T element : range) {
			Append(std::move(element));
		}
	}

	using RangeType = ArrayRange<ViewIntoTArray<T>, T>;
	inline RangeType Range() const { return { this }; }

	inline typename RangeType::Cursor begin() const { return Range().begin(); }
	inline typename RangeType::Cursor end() const { return Range().end(); }

private:
	ViewIntoTArray() {}
};

/// Used for owned `TArray<T>` such as those returned by functions.
template<typename T>
struct TArray
{
	T* dataPtr = nullptr;
	int32_t length = 0;
	int32_t capacity = 0;

	TArray() {}
	~TArray() { appFree(dataPtr); }

	TArray(TArray<T>&& other)
	{
		dataPtr = other.dataPtr;
		length = other.length;
		capacity = other.capacity;
		other.dataPtr = nullptr;
		other.length = 0;
		other.capacity = 0;
	}

	const T& operator[](size_t i) const { return dataPtr[i]; }
	T& operator[](size_t i) { return dataPtr[i]; }

	using RangeType = ArrayRange<TArray<T>, T>;
	inline RangeType Range() const { return { this }; }

	inline typename RangeType::Cursor begin() const { return Range().begin(); }
	inline typename RangeType::Cursor end() const { return Range().end(); }
};

}

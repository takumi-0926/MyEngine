#pragma once

template <class T>
inline void safe_delete(T*& t) {
	delete t;
	t = nullptr;
}
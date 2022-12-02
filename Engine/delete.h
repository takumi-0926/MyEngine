#pragma once

template<class T>
inline void _delete(T*& p) {
	//delete &p;
	p = nullptr;
}
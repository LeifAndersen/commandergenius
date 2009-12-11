/*
 * CSingleton.h
 *
 *  Created on: 19.04.2009
 *      Author: gerstrong
 */

#ifndef CSINGLETON_H_
#define CSINGLETON_H_

#include <stdio.h>

template <class T>
class CSingleton {
	
	
protected:
	static T *m_pSingleton; // Static Object
	
public:
	//CSingleton();
	inline static T* Get()
	{
		if(!m_pSingleton)
			m_pSingleton = new T;
		
		return (m_pSingleton);
	}
	
	static void Del()
	{
		if(m_pSingleton)
		{
			delete (m_pSingleton);
			m_pSingleton = NULL;
		}
	}

	virtual ~CSingleton() // Unused, because this class is never constructed!!
	{}
};

template <class T>
T* CSingleton<T>::m_pSingleton = NULL;

#endif /* CSINGLETON_H_ */

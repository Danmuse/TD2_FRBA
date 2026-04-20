/*
 * assert_hal.h
 *
 * Created: 27/1/2025 11:09:16
 *  Author: ddimodica
 */ 


#ifndef ASSERT_HAL_H_
#define ASSERT_HAL_H_

#ifdef NASSERT_HAL
	#define DEFINE_THIS_FILE_NAME 
	#define ASSERT(test_)((void)0)
#else
	#define DEFINE_THIS_FILE_NAME static char const l_this_file[]=__FILE__;
	#define ASSERT(test_)((test_)? (void)0 : onAssert(l_this_file, __LINE__));
#endif

void onAssert(char const * file, int line);

#endif /* ASSERT_HAL_H_ */

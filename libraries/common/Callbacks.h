#pragma once

/// Generic Callback function used to make algorithms interumpable
/// Return value: true continue, false break
typedef bool CallBack( const char * str );

/// Callback for progress bar: to know where we are
typedef bool CallBackPos(const int pos, const char * str );

inline bool DummyCallBack( const char * ) {return true;}
inline bool DummyCallBackPos(const int ,  const char * ) {return true;}

/// interruptible function 
typedef void TaskFunc(int t,void * contest); 

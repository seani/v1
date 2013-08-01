#pragma once

//
//One of our compiler id's must be defined
//

#if defined(__COMPILER_MSVC)
#else
    #error One of [ __COMPILER_MSVC ] compiler id's must be defined.
#endif

//
//One of the release configuration id's must be defined
//

#if defined(__CONFIG_DEBUG)
    #if defined(__CONFIG_RELEASE) || defined(__CONFIG_FINAL)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#elif defined(__CONFIG_RELEASE)
    #if defined(__CONFIG_DEBUG) || defined(__CONFIG_FINAL)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#elif defined(__CONFIG_FINAL)
    #if defined(__CONFIG_DEBUG) || defined(__CONFIG_DEBUG)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#else
    #error One of [ __CONFIG_DEBUG | __CONFIG_RELEASE | __CONFIG_FINAL ] configuration id's must be defined.
#endif

//
//One of the platform id's must be defined
//

#if defined(__PLATFORM_WIN32_PC)
#else
    #error One of [ __PLATFORM_WIN32_PC ] platform id's must be defined.
#endif


//
//
//do compiler specific things.
//
//

#ifdef __COMPILER_MSVC

//turn off dumb warnings
#pragma warning(disable: 4710) //function '' not inlined
#pragma warning(disable: 4514) //'' : unreference inline function has been removed
#pragma warning(disable: 4100) //'' : unreference formal parameter
#pragma warning(disable: 4512) //'' : assignment operator could not be generated
#pragma warning(disable: 4505) //'' : unreferenced local function has been removed
#pragma warning(disable: 4748) ///GS can not protect parameters and local variables from local buffer overrun because optimizations are disabled in function

#endif


//
//  debug.h
//  GIF-T
//
//  Created by Alexander Hill on 12/29/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#ifndef GIF_T_debug_h
#define GIF_T_debug_h

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#endif

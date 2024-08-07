# IWC lib

IWC lib is a general purpose C library, currently implementing allocators and
basic linear algebra. 

## Usage

This library for the most part uses [STB-style
headers](https://github.com/nothings/stb). That means that all the
implementations for the declarations in the header are also in that same
header, but guarded with a define you should put in exactly one file, like so:

```c
#define IWC_MEM_IMPLEMENTATION
#include "iwc_mem.h"
```

## Status

Current feature status is as follows:

- [ ] Allocators
    - [X] C `malloc` wrapper
    - [X] stack / buffer allocator
    - [ ] Tracking allocator
- [ ] Linear algebra
    - [X] Basic operations (+, -, x)
- [ ] Dynamic Arrays / Strings

## Questions

### Why is it named IWC?

This was originally derived from one of my other projects [Interworld
Trader](https://github.com/Kartoffelsaft/interworld). The "IW" part of it is
based on the "**I**nter**W**orld" naming. The "C" is from this being somewhere
between an extension and replacement for libc.

### Why make this?

Having used Zig and Odin before, one of the most convenient features of those
languages is allocators. That isn't inherently impossible with C however, but
from my searching I could not find any allocators that provide that interface.

Combine that with projects that are very memory constrained and/or have trouble
linking with libc (like [Interworld
Trader](https://github.com/Kartoffelsaft/interworld)), I found myself writing
some very simple allocators and linear algebra by hand. I figured it would be
convenient to have what I wrote in a place that is easy to use for other
projects that I may try, as well as other people.

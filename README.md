### Status
[![Build Status](https://semaphoreci.com/api/v1/xoorath/xo-math/branches/master/badge.svg)](https://semaphoreci.com/xoorath/xo-math)

xo-math is currently in progress and not ready for production use.

# Quick start guide

Download and include `xo-math.h` from the head revision of the repo. No other files are required to use xo-math.

Once xo-math is included, you can begin using the math types included. Example:

```

#include <iostream>
#include "xo-math.h"

using namespace std;
using namespace xo;
using namespace xo::math;

int main() {
  
  Vector3 hello = Vector3::RandomOnSphere();
  Vector3 world = Vector3::RandomInConeRadians(Vector3::Forward, HalfPI/2.0f);

  cout << hello << endl << world << endl;

  return 0;
}

```

_note:_ The only header to ever be included for xo-math is `xo-math.h`, even when using the sources from the `xo-math/include/` directory.


# Configuration

All configuration can be done with defines before including `xo-math.h`. It's suggested that you wrap xo-math.h in another header that does the config for you if you have configuration to be done. Something along the lines of:

```
project/xo-math/include/xo-math.h   | the real xo-math
project/xo-math/xo-math.h           | contains configurations and includes xo-math
```

## SIMD configuration

xo-math supports [SIMD](https://en.wikipedia.org/wiki/SIMD). If you've configured your compiler explicitly for x86 simd instructions then xo-math should detect that automatically.

See the [/arch option for visual studio](https://msdn.microsoft.com/en-us/library/7t5yh4fd.aspx).
See the [extended instruction switches for gcc](https://gcc.gnu.org/onlinedocs/gcc-4.5.3/gcc/i386-and-x86_002d64-Options.html). Search the page for `-msse4.2` and similar. 
These flags also work with supported clang versions.

If you know what you're doing, you can also explicitly define simd support manually. *If you do define any of these, xo-math expects you to define all the simd defines you want, not just the highest version.*

```
#define XO_SSE 1
#define XO_SSE2 1
#define XO_SSE3 1
#define XO_SSSE3 1
#define XO_SSE4_1 1
#define XO_SSE4_2 1
#define XO_AVX 1
#define XO_AVX2 1
```

If you do define any of the above, but our detection of your compiler settings disagrees with your defined configuration - we will warn you about it at compile time unless you define the following:

```
#define XO_NO_VECTOR_DETECT_WARNING 1
```

## Namespace configuration

### Custom namespace

```
#define XO_CUSTOM_NS my_namespace
#include "xo-math.h"
...
my_namespace::Vector3 v;
```

### Single namespace

```
#define XO_SINGLE_NS
#include "xo-math.h"
...
xo::Vector3 v;
```

### Simple namespace

```
#define XO_SIMPLE_NS
#include "xo-math.h"
...
xomath::Vector3 v;
```

### No namespace

```
#define XO_NO_NS
#include "xo-math.h"
...
Vector3 v;
```

### Without configuration
```
#include "xo-math.h"
...
xo::math::Vector3 v;
```

## No inverse division

*Note:* This option only has an effect when SSE is enabled. The documentation following in this section is only true when SSE is enabled.

To speed up vector division, we do an approximation. It's fairly inaccurate (somewhere on the order of a `0.00036` error), but is between 1.3x and 3.3x faster.

This optimization can be disabled with the following define:

```
#define XO_NO_INVERSE_DIVISION
#include "xo-math.h"
```
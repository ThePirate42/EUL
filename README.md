# EUL
The mathEmatics Useful Library (the name is a work in progress) is a math general purpose c++20 header library that, among other things, features a big integer implementation. It was initially created for personal use while solving [Project Euler](https://projecteuler.net/) problems. That is kinda still my only reason of updating this for now. It's probably one of the least performant implementations of bigint out here, but feel free to use it if you want, at least it's very simple to "install" and use.

## How to use

For starters, everything in the library is placed inside the `eul` namespace. There is one exception: the `bigint operator"" _bi (const char *n)` function, the function responsible for implementing bigint literals, is inside the inline `eul::literals` namespace; the reasons for this choice are explained in [this](https://web.archive.org/web/20210802172056/https://stackoverflow.com/questions/49054477/how-to-refer-to-user-defined-literal-operator-inside-a-namespace) StackOverflow page.

The library uses three headers:

- `typesupport.hpp`  
This header features some useful concepts, that have equivalents inside the standard but are aware of the custom types of the library, and a typedef for `std::vector<bool>` that I may use in my implementation.

- `eulalgorithm.hpp`  
This header features several generic functions, useful to compute stuff usually related to number theory.  
The `isqrt` function (integer square root, in other words `std::floor(std::sqrt(n))` but doesn't use floating point internally) has an alternative implementation, named `isqrtalt`; it's slower, but it's constexpr. If for some reason you want the other functions in the library to use the alternative implementation, compile with `-D ALTISQRT`: the two implementations will exchange names.

- `bigint.hpp`  
This header implements the `bigint` class, several related functions, as well as overloads of some of `eulalgorithm.hpp`'s functions.

### Bigint guide

To initialize a bigint, simply write

```
eul::bigint a;
```

or

```
eul::bigint a = b;
```

The `bigint` class has a template constructor that accepts all the integer types

```
eul::bigint a = 747348;
```

and an *explicit* template constructor that accepts floting_point types

```
eul::bigint(7.563);
```

There is also `stobi`, a string to bigint function that works like `std::stoi` (but accepts only base 10 as input), and an explicit conversion function bigint to string (that also only has base 10 output). For now, there **isn't** a bigint to integer conversion function (I'll add it when I'll decide how to handle numbers that the builtin types can't handle).

If you want to initialize a bigint with an integer literal, but with a number greater than what the builtin integer types can handle, you can use a custom literal: just append `_bi` to the literal in question, like this:

```
eul::bigint a = 747348678936798367396739673967936793678_bi;
```

Just remember to extract the custom literal function in the global namespace before, for example with `using namespace eul::literals;`, more details in the first paragraph after "How to use".

The class also supports:

- contextual conversion to bool.

- all comparison operators (included three-way).

- all increment/decrement operators.

- unary `+` and unary `-`.

- `+`, `-`, `*`, `/`, `%` and the corresponding compound assignment operators.

- `<<` and `>>` for stream insertion/extraction.

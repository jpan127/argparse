# Description
A C++14 compliant command line parser.

# Implemented Features
- Strongly typed command line options
- Letter-based or string-based options
- Enforcing requirement of specific options
- Allowed values for a specific option
- Default values for a specific option
- Subparsing, various parsers that depend on the value of a preceding option

# Build Status

Branch  | Build Status
------- | -------
master  | [![Build Status](https://travis-ci.org/jpan127/argparse.svg?branch=master)](https://travis-ci.org/jpan127/argparse)
feature | [![Build Status](https://travis-ci.org/jpan127/argparse.svg?branch=feature)](https://travis-ci.org/jpan127/argparse)

# Supported Compilers

OS     | Compiler
------ | ------
Linux  | Clang 8.X

# Usage

## 1. Create a `Parser` object
```c++
argparse::Parser p("my name", "my cool program");
```

## 2. Add options

### Add a non positional option.

- Starting value is "yaml" if no values are provided
- Provided value must be either `cpp`, `yaml`, or `py`
- The option can be specified with `--type <value>` or `-t <value>`
- The help text is displayed in a table when `-h`, `--help` is provided or on error
- The required flag tells the parser to enforce any options to be present

```c++
const auto type = p.add<std::string>({
    .default_value = "yaml",
    .allowed_values = {"cpp", "yaml", "py"},
    .name = "type",
    .help = "Type of the input file",
    .required = true,
    .letter = 't',
});
```

### Add a positional option.

- The configuration applies the same as a non positional
- The positional arguments are expected in the order they are added

```c++
const auto first  = p.add_leading_positional<std::string>({ .name = "first"  });
const auto second = p.add_leading_positional<std::string>({ .name = "second" });
const auto third  = p.add_leading_positional<std::string>({ .name = "third"  });

// Expected call
<PROGRAM_NAME> <first VALUE> <second VALUE> <third VALUE>
```

### Add a multivalent option.

- The configuration applies the same as a non positional
- This option can have multiple values
- It will parse all the values after the `--name` or `-letter` flag until the next flag

```c++
const auto values = p.add_multivalent<std::string>({ .name = "values" });

// Expected call
<PROGRAM_NAME> --values value1 value2 value3
```

## 3. Parse the arguments

- After all options are registered, the command line arguments need to be parsed
- Any additional arguments provided after `--` will be stored in a vector returned by `parse`

```c++
const auto &remaining_arguments = p.parse(argc, argv);
```

## 4. Use the parsed arguments

- Every `add` function returns a `ConstPlaceHolder<T>`
- Every `add_multivalent` function returns a `ConstPlaceHolder<std::vector<T>>`
- This placeholder is a `std::shared_ptr<optional<T>>`
- The `std::shared_ptr` wrapper is so that the value can be populated at parsing
- The `optional` second wrapper is to signify whether there exists a value
- This library uses `tl::optional` as the optional implementation since C++14 does not support it yet [tl::optional](https://github.com/TartanLlama/optional)

```c++
// Example option
const auto option = p.add<std::string>({ ... });

// We can assume the pointer is valid
assert(option);

// First we can check if the value exists
if (option->has_value()) {
    ...
}

// Next we can get a const reference to the value
const auto &value = option->value();

// Or use it directly
if (option->value() == "some expected value") {
    ...
}
```

## What happens on failure?

- On any failure, errors are printed in red of the cause
- The usage, description, and information about the options are printed
- Then the program will throw a `std::runtime_error` exception

This is the help text from the sample program:

```c++
Usage: Sample Program [mode] [path] [--verbose] [--h] [--id] [--help]
Description: Testing...

Options:
---------------------------------------------------------------------------------------------------
|Required| Name  |Letter|  Type  |Default|             Help             |     Allowed Values      |
---------------------------------------------------------------------------------------------------
|        |  id   |      |int64_t |       |  some identification number  | 1234 , 9999 , 127127127 |
|        |verbose|      |uint64_t|   5   |                              |                         |
|   x    | path  |      |  bool  |   0   |                              |                         |
|   x    | mode  |      | string |       |Operation mode of this program|        a , c , b        |
|        | help  |  h   |  bool  |   0   |      Show help message       |                         |
---------------------------------------------------------------------------------------------------
```

## Subparsers

- Suppose we wanted a specific argument to determine what options to parse
- For example we want different options for 3 different modes
- We can create this by using a subparser, then adding the different options to the subparsers and not the parent parser
- The value that the subparser will switch on is treated like a leading positional argument
- It must be specified first, prior to other options

```c++
// Create subparsers
auto &subparsers = p.add_subparser("mode", {"read", "write", "append"});

// Split the subparsers into individual parsers
auto &read   = subparsers["read"];
auto &write  = subparsers["write"];
auto &append = subparsers["append"];

// Add options for each subparser
const auto read_only_option = read.add<std::string>({ .name = "read_only_option" });
const auto write_only_option = write.add<std::string>({ .name = "write_only_option" });
const auto append_only_option = append.add<std::string>({ .name = "append_only_option" });

// Expected call
<PROGRAM_NAME> <MODE> <MODE OPTIONS>
./sample read --read_only_option option_value
./sample write --write_only_option option_value
./sample append --append_only_option option_value
```

## Supported Types

All of the above examples use `std::string` as the option type but all fundamental types are supported as well.

## More Information

To read more about how the library works, you can start with [argparse.h](argparse/include/argparse.h).

# Coding Standard

The goal of establishing a coding standard within the company is to give a uniform appearance to a codebase written by different engineers. A coding standard improves readability and maintainability of the code.

The coding standard will be grandfathered in. Existing code won’t be refactored, but we ask that the standard be applied for any new code or modified/ported/refactored code.

For a complete example of a `.c` and a `.h` that follow the standard, see this page: [`.c` and `.h` Examples that closely follow the coding standard](link-to-example)

## Variable and Object Naming

- DO NOT over shorten variables/function names. The compiler removes the name anyway; your goal therefore must be to make the code readable.
  - e.g., `QSPIEn` → `isQSPIEnable`
- DO include units in variable names.
  - e.g., `requestTimeout` → `requestTimeoutInMs`

### Local Variables and Parameters

- DO use camel casing
- DONT use `_` in those variable names.
- Start with lower case
  - e.g., `fastLoopMD_handle` → `fastLoopMdHandle` or `fastLoopMDHandle`

### Global Variables

- DO use Pascal casing.
- Avoid using global variables if possible.

### Structs

- DO use Pascal casing for the name, with the module name in it.
  - e.g., `ThrottleHandle`
- DO use camel casing for members of the struct.

### Enums and Constants

- DO define constants using the `const` keyword instead of `#define`, to make intent clear.
- DO define constants and enum members in caps, with '_' separating words.
  - e.g., `MAX_BUFFER_SIZE`
- DO NOT use magic numbers in the code. DO use constants.
  - e.g., `if (foo <= 22)` is bad. `if (foo <= BUFFER_LIMIT)` is good.

### Typedefs

- DO end the name with 't' to show it is a type. `PinSelect_t`

## Functions/Methode Declaration

- DO declare clear, concise function names and parameter names. Functions usually perform actions, so the name should be clear regarding that.
  - e.g., `ErrorCheck()` → `CheckForErrors()`
- For non-object-oriented languages, use `ModulePascalCase_FunctionPascalCase`.
  - e.g., `Disp_Init` → `Display_Init()` for public functions.
- The name of the module can be an abbreviation but it can't only contain capital letters. Example: 'Regular Conversion Manager' becomes 'RegConvMan' and not 'RCM,' and 'storage management' could become 'StorageMan.'
- DO NOT assume the return type of a function, always specify it.
- DO NOT return `void*` or any other "generic" pointer like this.
- DO use Pascal casing.
- DO use "object-oriented"-like behavior whenever possible.
  - e.g., It’s preferable to define a struct (or object) `Display` and then to call `Display->IsDisplayDetected`, than to have a global `Display_IsDisplayDetected` method that anyone anywhere in the code can call.
  
## Spaces, Newlines, Indentation

- Replace the "tab" by 2 spaces in your editor (keep default setting in VSCode). This enables spacing to look uniform across IDEs and on GitHub. Otherwise, alignment becomes off.
- Spaces:
  - Around parentheses:
    - DO insert a space before a parenthesis.
    - DO NOT insert a space after a parenthesis.
    - Example: `if ( condition == true )` → `if (condition == true)`
- Newlines, indentation:
  - Follow the indentation on the left in this document: [C/C++ Programming - Indentation Styles](indentation-link)
  - Newline after every line of code, aka newline after every ';'.
  - Don’t have two instructions on the same line.
  - Every scope of code needs its own indentation level.

## Curly Brackets

- Always add curly brackets, even for example if an 'if' condition only has one line.
- Always a new line for a curly bracket.
- DO NOT put a curly bracket on the same line as another instruction (e.g., if condition).

## Pointers

- DO initialize every pointer, every time (to NULL or other). Pointers that are left uninitialized and then end up being used by mistake can cause undefined behaviors that are very hard to debug.

## GOTO

- DO NOT use goto statements.

## Comments

- The best code is self-documented. Use meaningful function names and variables.
- Comments are there to structure your thoughts. Use them.
- Write comments imagining that they will end up in documentation (e.g., Doxygen-generated doc). A person should understand the system only by navigating through the documentation, without the code.
- You don’t write comments just for yourself but for the rest of the team, as well as for future you. You won’t remember in a year why you wrote the code you are writing today.
- When writing a new function, here’s a good methodology one can use to have meaningful comments:
  - Write the function name, and add a description to the method.
  - Then, before writing any code, write down comments for every logical step that the function must do.
  - e.g.,
    ```
    // Calculate X
    // Compute Y
    // Set value on module Z because xyz
    // Adjust setting RRR because yxz
    // Return the computed delta between Y and Z
    ```
  - Then, add the code that does what the comments say.

### Comments on Methods

- Header files need to follow Doxygen-like format, example below.
- Document how to use the function in the header file, where it’s declared.
  - Example:
    ```
    /**
      @brief Function used to read the state of a GPIO using the Renesas API
             Depending on if the pin is in output or input mode a different part of the pin register is being read
      @param Receives GPIO number
      @return void
    */
    bool uCAL_GPIO_Read(uint32_t aGPIO);
    ```

- Document how the function works (if it's not obvious from the code) in the source file (aka the .c file), where it’s implemented. This one doesn’t need to be Doxygen-like.
  - Example:
    ```
    /**
      Function used to read the state of a GPIO
    */
    bool uCAL_GPIO_Read(uint32_t aGPIO)
    {
        // Implementation details here
    ```

## Code Sanity Checks

- Use sanity checks to validate preconditions and postconditions of methods, such as expected argument values within expected ranges (e.g., pointers are not NULL, indexes are non-negative and less than a max limit, etc.), expected state, expected return value from called functions, etc.
- Sanity checks should:
  - Log and then stop program execution (i.e., an assert) in debug mode.
  - Try to recover (e.g., hardware reset) in release mode.
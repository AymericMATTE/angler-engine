# Naming Conventions
For the sake of consistency and clarity, this repository enforces strict naming rules for both the codebase and the GitHub history.
If you want to contribute, you must follow them.

If you spot any missing information on this page, please open an issue and point out the missing language feature, 
or the information that has not been covered.


## Code Conventions
The **AnglerEngine** codebase follows naming conventions inspired by the stdlib, sfml & raylib code styles :

| Code Element                   | Convention   | Example                               |
|--------------------------------|--------------|---------------------------------------|
| Preprocessor definitions (constants) | UPPER_CASE  | `#define MY_PREPROC 1`           |
| Preprocessor definitions (macros)    | UPPER_CASE  | `#define MY_MACRO(...)`          |
| Any variables (default if no conventions) | camelCase    | `int myVar = 0;`           |
| Local variables                | camelCase    | `int myLocalVar = 0;`                 |
| Global variables               | g_camelCase  | `int g_myGlobalVar = 0;`              |
| Static variables               | s_camelCase  | `int s_myStaticVar = 0;`              |
| Constant variables             | UPPER_CASE   | `const int MY_VAR = 0;`               |
| Constexprs / Constvals         | UPPER_CASE   | `constexpr int MY_VAR = 0;`           |
| Aliases (typedefs / structs)   | PascalCase   | `typedef int MyType;`                 |
| Structs & Classes              | PascalCase   | `class MyClass {...};`                |
| Public member variables        | camelCase    | `int myPublicVar;`                    |
| Private member variables       | m_camelCase  | `int m_myPrivateVar;`                 |
| Functions / Methods            | camelCase    | `int myFunc(...);`                    |
| Function parameters            | _camelCase   | `int myFunc(int _myParam);`           |
| Callback Functions / Events    | PascalCase   | `int OnMyEvent(...);`                 |
| Enums                          | PascalCase   | `enum MyEnum {...};`                  |
| Enum members                   | UPPER_CASE   | `enum MyEnum { MY_ENUM_VALUE };`      |

The specified code style must also be followed :
- Floats must always be defined in the format `x.xf`, for example : `1.0f`.
- All operators must be separated by one space on each side, for example : `10 * 10`.
- Ternary operators are included in the rule above,  for example : `(condition) ? result1 : result2`.
- Pointers must have the `*` symbol attached to the type, followed by a space, for example: `MyType* ptr = NULL;`.
- All code-related files should use `CRLF` line endings and `space indentation`.

Additionally, for writing functions and control flow blocks :
- The opening curly brace `{` goes on the same line as the function or statement.
- Leave a space between the keyword (like the function's return type or if) and the opening parenthesis `(`.
- Leave a space between the closing parenthesis `)` and the opening curly brace `{`.

Do ✅ :
```C++
int myFunc() {
	if (1) {
		// Do something
	}
}
```
Don't ❌ :
```C++
int myFunc()
{
	if(1)
	{
		// Do something
	}
}
```

On a final note, try to be concise yet precise in your function names.
For variables, one-word names are preferred.


## Commits Conventions
The **AnglerEngine** commit conventions are inspired by the PRs of raylib contributors.

A commit message MUST conform to the following format :
`[module] {action verb} {information} {data} {additional information}`

With :
- `module` : the part of the code being worked on (ex: gui, activator, automation tools, documentation, etc.)
- `action verb` : an action verb describing what the commit is about (ex: added, deleted, updated, fixed, etc.)
- `information` :  information about the action verb (ex: added what?, deleted what?, fixed what?, etc.)
- `data` : if required, the file / function in which the action has been made (ex: added in what function?, deleted in what file?, etc.)
- `additional information` : if required, additional information on the data (ex: what does the function edited do?, etc.)

Additionally, the following constraints must be complied with :
- Commit messages should not be longer than 75 characters, try to be as concise as possible.
- Following the constraint above, if you can't describe everything you want to in the message, add a description to your commit.
- Each commit should start with a capitalized action verb and should not end with a period.

Some examples :
- `[activator] Added implementation of AutoFindWindowsDist() function`
- `[tools] Updated autobuild system for resourcepacker.h`
- `[tests] Fixed typos in unit tests 2 & 3`
- `[documentation] Updated code conventions`

## File Structure
The **AnglerEngine** file structure, follows principle mostly used in web and C :

| Element                   	 	| Convention   	| Example                               |
|-----------------------------------|---------------|---------------------------------------|
| Directory 						| kebab-case  	| `my-directory`           				|
| File (default if no conventions)   | kebab-case  	| `my-file.txt`          				|
| Header file (default if no conventions)   | kebab-case  	| `my-header.h`          		|
| CPP file (default if no conventions)   | kebab-case  		| `my-header.cpp`          		|

## Consequences
Any team member or contributor who does not respect the naming conventions will have their PR rejected until the mistake is corrected.
If those mistakes are repeated too often, the person may have their ability to push or create PRs temporarily revoked, as we don't want to spend all our time doing DevOps for people who don't know how to read.
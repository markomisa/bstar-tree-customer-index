# B*-Tree Customer Index Project

This project implements a B*-Tree structure in C++ to manage and index `Customer` records using their primary key (ID). It utilizes CMake for cross-platform compilation.

## Project Structure

* `main.cpp`: Main entry point for the command-line interface.
* `customer.h`, `customer.cpp`: Defines and implements the `Customer` class and file reading logic.
* `treenode.h`, `treenode.cpp`: Defines the internal structure of a single tree node.
* `bstartree.h`, `bstartree.cpp`: Defines and implements the core B*-Tree logic (insertion, search, splitting, etc.).
* `CMakeLists.txt`: Build script for CMake.

## Building the Project (Using CMake)

To compile the project, follow these standard steps from the root directory of the repository:

1.  **Create a build directory:**
    ```bash
    mkdir build
    ```

2.  **Navigate into the build directory:**
    ```bash
    cd build
    ```

3.  **Configure the project using CMake:**
    ```bash
    cmake ..
    ```

4.  **Compile the executable:**
    ```bash
    make
    ```

A runnable executable named **`bstree_main`** will be created inside the `build/` directory.

## Running the Application

Execute the compiled program from the `build` directory:

```bash
./bstree_main
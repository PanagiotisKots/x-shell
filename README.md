# XShell (Extended Shell)

![GitHub release (latest by date)](https://img.shields.io/github/v/release/your-username/xshell?style=flat-square)
![GitHub last commit](https://img.shields.io/github/last-commit/PanagiotisKots/xshell?style=flat-square)
![GitHub issues](https://img.shields.io/github/issues/your-username/linux-shell?style=flat-square)
![GitHub pull requests](https://img.shields.io/github/issues-pr/your-username/xshell?style=flat-square)

---

## **Project Overview**

**XShell** is a custom Unix shell designed specifically for Linux distributions. It provides basic shell functionality with enhanced features such as job control, command history, and pipelining. Built to function within the terminal, XShell supports the execution of both built-in commands and external programs, allowing users to interact with the system and manage processes.

---
## **Key Features**

| Feature           | Description                                                                                                       |
|-------------------|-------------------------------------------------------------------------------------------------------------------|
| **Job Control**    | *XShell* allows users to run processes in the background and bring them to the foreground as needed. Commands like `jobs`, `fg`, and `bg` enable efficient process management. |
| **Command History**| With support for command history, *XShell* enables users to recall and execute previously run commands using the arrow keys or the `history` command. |
| **Built-in Commands** | *XShell* includes several built-in commands for common tasks: <br> `cd`: Change the current directory. <br> `help`: Display help information about the shell. <br> `exit`: Exit the shell. <br> `history`: View the history of executed commands. <br> `jobs`: List all background jobs. <br> `fg`: Bring a background job to the foreground. <br> `bg`: Resume a stopped job in the background. |



- **External Command Execution**:  
  *XShell* can execute external programs and commands, making it versatile for interacting with the operating system. It also supports the execution of processes in the background by appending `&` to a command.

---

## **Design and Purpose**

*XShell* is designed to be lightweight and functional, offering a simplified yet powerful alternative to more complex shells like **Bash** or **Zsh**. It has been fully tested on Debian-based Linux distributions and aims to provide an intuitive experience for users who need basic shell capabilities with added support for job control and history.

This shell is ideal for users who want to manage processes and commands efficiently without the overhead of a more feature-heavy environment. *XShell* is built with simplicity in mind, offering just the right amount of functionality for most day-to-day tasks in a Linux terminal environment.





# Installation

**XShell** requires both the GNU Readline and History libraries to handle command input, history, and line editing. The installation process may vary depending on your Linux distribution. Follow the instructions below to install the necessary dependencies and run XShell.

## 1. Install Dependencies

### Ubuntu/Debian-based Distributions

For Ubuntu or other Debian-based distributions, install the necessary development libraries:


    sudo apt update
    sudo apt install libreadline-dev libhistory-dev

### Red Hat/Fedora-based Distributions

On Fedora, Red Hat, or CentOS, use dnf to install the required libraries:

    sudo dnf install readline-devel history-devel

### Arch Linux-based Distributions

On Arch Linux or Manjaro, use pacman to install the libraries:

    sudo pacman -S readline

(Note: Arch-based distributions generally include history functionality with readline, so a separate libhistory package is not needed.)


## 2. Compile XShell

After installing the dependencies, compile XShell using gcc, linking both readline and history libraries:



    gcc -o xshell shell.c -lreadline -lhistory

This command compiles the shell.c file and links the readline and history libraries to create the executable xshell.


## 3. Running XShell

Once compiled, run XShell with the following command:


     sudo su
     
    ./xshell

## 4. Uninstallation

To remove the installed dependencies:
Ubuntu/Debian-based:

    sudo apt remove libreadline-dev libhistory-dev

Fedora-based:

    sudo dnf remove readline-devel history-devel

Arch Linux-based:


    sudo pacman -R readline

Basic Usage

## 1. Changing Directories

Use the cd command to change the current working directory:


    cd /path/to/directory

## 2. Running External Programs

You can run any external program, including running processes in the background by appending &:



    ./my_program &

## 3. Managing Jobs

XShell provides built-in job control:

    List Jobs:


jobs

    Bring Job to Foreground:


fg PID

    Send Job to Background:


bg PID

## 4. Command History

XShell uses the History library to manage command history:

    View History:


history

    Recall Commands:
    Use the up/down arrow keys to navigate previously entered commands.

## 5. Exiting XShell

To exit XShell, use the exit command:

exit

Troubleshooting
Common Errors

    readline/readline.h or history/history.h not found:
    Ensure you have installed both libreadline-dev and libhistory-dev (or the equivalent for your distribution).

    Compile Errors:
    Ensure both readline and history libraries are properly linked during compilation:


gcc -o xshell shell.c -lreadline -lhistory







# Contributing to XShell

Thank you for considering contributing to **XShell**! We welcome all contributions, whether you're fixing bugs, adding new features, improving documentation, or optimizing existing code. Below are the guidelines to help ensure smooth collaboration on this project.

## How to Contribute

1. **Fork the Repository:**  
   Start by forking this repository to your own GitHub account.

2. **Clone the Fork:**  
   Clone the forked repository to your local machine.


       git clone https://github.com/your-username/xshell.git

       Create a Branch:
       Create a new branch for your feature or bug fix. Ensure your branch name is descriptive:


Make Changes:
Implement your changes. If you’re adding a new feature or fixing a bug, please ensure:

    Your code is well-commented and follows the project's coding style.
    New features come with appropriate tests (if applicable).

Run Tests:
Ensure that all existing and new tests pass before submitting your changes. You can also manually test your modifications by running XShell locally.

Commit Changes:
Commit your changes with a clear, concise commit message:





   Submit a Pull Request:
   Go to the original repository and submit a pull request (PR) from your branch. Please include the following in your PR:
         
        A detailed description of what the changes do.
        Reference to any relevant issues or discussions.
        Screenshots or logs if applicable.

Once your PR is submitted, it will be reviewed. Be patient as maintainers may request changes or clarification before merging.

Code Style Guidelines

    Follow consistent indentation (use spaces, preferably 2 or 4).
    Make sure variable and function names are descriptive and written in snake_case.
    Use comments to explain complex logic.
    Keep commits small and focused on a single task or feature.
    Avoid unnecessary libraries or dependencies.

Reporting Bugs

If you encounter any issues or bugs, feel free to report them by creating a new issue on the repository. When filing a bug report, please provide the following information:

    A clear description of the issue.
    Steps to reproduce the bug.
    Any error messages or logs.
    Your system environment (OS version, distribution, shell version).

Feature Requests

We also welcome feature requests! If you have an idea for a new feature or improvement, please open a new issue with the following details:

    A description of the proposed feature.
    How the feature would benefit users or improve XShell.
    If possible, provide a technical outline or pseudocode.

Community and Code of Conduct

We strive to create a welcoming, friendly, and collaborative environment. Please be respectful and constructive in all your interactions. Any behavior that does not adhere to these values may result in the removal of contributions or exclusion from future participation in the project.
License

By contributing to this project, you agree that your contributions will be licensed under the same terms as the project itself, which is open-sourced under the MIT License.

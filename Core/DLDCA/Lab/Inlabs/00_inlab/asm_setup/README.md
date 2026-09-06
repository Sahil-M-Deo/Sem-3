## Assembly

We will be using `x86_64` assembly standard for the duration of the course. 

People using an `x86_64` processor can run this natively, but if not, you will be provided access to `mars`/`sl machines` where `nasm` is installed.

### Installing nasm
```bash
sudo apt install nasm
```
Do this only if running `make` throws an error.

### Compiling Assembly Programs

Assembly compilation is done in 2 steps:
1. Creation of object files
2. Linking of object files

```bash 
nasm -f elf64 <file-name> -o <output-object-file>
ld <object-files> -o <executable>
```

For ease, you have been given a `Makefile`. You can just run `make` and check the expected output.

### Logging into mars/sl-machines & Transferring Files

To connect to **mars**, you can use `ssh` to open a command-line interface. You can edit code directly on the remote server using an editor like `vim`, or write your code locally and transfer files back and forth using `scp` (Secure Copy Protocol).

#### 1. SSH into the Server
```bash
<your-machine>$ ssh <cse-username>@mars.cse.iitb.ac.in
```

#### 2. Transferring Files using `scp`

> **Important:** Always run `scp` commands from your **local terminal**, not from inside the active `ssh` session on `mars`.

* **Upload a local file to `mars`:**
  ```bash
  scp <path-to-local-file> <cse-username>@mars.cse.iitb.ac.in:<remote-path>

  # Example: Upload program.asm to your home directory on mars
  scp program.asm <cse-username>@mars.cse.iitb.ac.in:~/
  ```

* **Download a file from `mars` to your local machine:**
  ```bash
  scp <cse-username>@mars.cse.iitb.ac.in:<remote-file> <local-destination>

  # Example: Download the compiled binary back to your current local directory (.)
  scp <cse-username>@mars.cse.iitb.ac.in:~/my_executable .
  ```

* **Transfer an entire directory (use the `-r` recursive flag):**
  ```bash
  # Upload a local project folder to mars
  scp -r ./lab1_folder <cse-username>@mars.cse.iitb.ac.in:~/
  ```
  


## Verilog
We will be using `icarus-verilog` (iverilog) for simulations and [VerilogHDL/SystemVerilog extension](https://marketplace.visualstudio.com/items?itemName=mshr-h.VerilogHDL) for Syntax-Highlighting and [VaporView](https://marketplace.visualstudio.com/items?itemName=lramseyer.vaporview) for observing the waveforms.


You can install iverilog using your package manager if you are using Linux/macOS. *For windows we recommend using WSL*, but if you insist you can check [iverilog for windows](https://bleyer.org/icarus/).

### Ubuntu / Debian / WSL
```bash
sudo apt update
sudo apt install iverilog
```

### Fedora / RHEL
```bash
sudo dnf install iverilog
```

### MacOS
```bash
brew install icarus-verilog
```
### Installation verification
Check the version of iverilog to confirm it's properly installed
```bash
iverilog -v
```


### Running Simulations

1. **Compile**: Create an executable simulation file from your Verilog source (`design.v`) and testbench (`testbench.v`):
    
    ```bash
   iverilog -o <output-file> <input-files>
   ```
2. **Execute**: Run the simulation to generate the Value Change Dump (`.vcd`) file:
   ```bash
   vvp <output-file>
   ```
3. **View**: Open the resulting `.vcd` files directly in VSCode works once the extension is installed

You are given a codebase in `system-verilog` (a superset of verilog) for checking the compilation and learning to navigate the waveforms. **You are in no way expected to understand all the given code, but it can serve as a spot you can explore system-verilog.**

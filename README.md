# devenv for nix

a tool to quickly set up development environments using nix-shell and presets

## features

- **manage presets**: create, list, and remove development environment presets
- **quick initialization**: spin up new projects with a single command
- **nix-shell integration**: automatically generates `shell.nix` files
- **git support**: optionally initializes git repositories in new projects

## building

```bash
./build.sh
```

the compiled binary will be available in `./build/devenv`

## usage

### presets

manage your development environment presets:

#### list all presets
```bash
./devenv presets
# or
./devenv preset list
# or
./devenv preset ls
```

displays all saved presets with their descriptions and packages

#### add a new preset
```bash
./devenv preset add <preset-name>
```

you'll be prompted to enter:
- a description of the preset
- packages to include (comma-separated)

example:
```bash
./devenv preset add rust
# Enter description: 
# > Rust development environment
# Enter packages (comma separated): 
# > rustc,cargo,rust-analyzer
```

#### view preset details
```bash
./devenv preset cat <preset-name>
```

shows the description and all packages in a specific preset

#### remove a preset
```bash
./devenv preset rm <preset-name>
```

### initialization

create a new project using a preset:

```bash
./devenv init <preset-name> [directory]
```

if no directory is specified, you'll be prompted to enter one, the same way as with presets.

the command will:
1. create the directory
2. generate a `shell.nix` file with your preset packages
3. initialize a git repository (if git is available)
4. provide instructions for starting development

example:
```bash
./devenv init rust ~/projects/my-rust-app
```

once created, enter your development environment:
```bash
cd ~/projects/my-rust-app
nix-shell
```

## examples

**create a nodejs preset and initialize a project:**
```bash
./devenv preset add node
# Enter description: Node.js development environment
# Enter packages: nodejs,npm,yarn

./devenv init node ~/projects/web-app
cd ~/projects/web-app
nix-shell
```

**create a python data science preset:**
```bash
./devenv preset add datascience
# Enter description: Python data science environment
# Enter packages: python311,python311Packages.numpy,python311Packages.pandas,python311Packages.matplotlib

./devenv init datascience ~/projects/data-analysis
```

## storage

presets are stored in `~/.local/share/devenv/presets.json` and can be edited manually if needed

## requirements

- linux/unix system
- nix with nix-shell available
- git (optional, for automatic repository initialization)
- C++20 compatible compiler
- cmake
- nlohmann/json library

(or, just use `shell.nix` to set up the development environment for this tool itself, how ironic!)

Happy hacking! 🚀
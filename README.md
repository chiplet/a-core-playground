# Quickstart

This is still a very ad-hoc setup. Documenting build commands that work on my machine, but these will not work straight out of the box for anyone else :)

## Setup development environment
bash:
```
distrobox enter test0-racklet-hw-env
```

distrobox:
```
export PATH=$PATH:/prog/riscv/bin
```

## Build ACoreChip with enough memory
```
cd ACoreChip
./configure && make core_config=core-doom.yml gen_cheaders=true
```

## Run Verilator Simulation
```
cd tb-verilator

# install verilator if running for the first time
scripts/install_verilator.sh

source ~/venv/bin/

# define VERILATOR_ROOT and such
source sourceme

make
```
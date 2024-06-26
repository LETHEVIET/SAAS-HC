# Self-Adaptive Ant System with Hierarchical Clustering for the Thief Orienteering Problem
This repository implements the SAAS-HC algorithm (Self-Adaptive Ant System with Hierarchical Clustering) for the Thief Orienteering problem. The algorithm explanation was presented in [the paper](https://doi.org/10.1145/3628797.3629024) "Self-Adaptive Ant System with Hierarchical Clustering for the Thief Orienteering Problem" by Vu Hoang Huynh, The Viet Le, and Ngoc Hoang Luong.

Furthermore, this repository provides the SAAS-HC solution for the ThOP benchmark and the solutions of other algorithms: ILS, BRKGA, ACO, and ACO++. The implementations of these algorithms can be found in [this repository](https://github.com/jonatasbcchagas/acoplusplus_thop). All the solution files were stored in the experiments folder.
## Contributor
[Vu Hoang Huynh](https://orcid.org/0009-0003-3465-730X), [The Viet Le](https://github.com/LETHEVIET/), and Ngoc Hoang Luong.
## Installation
### Linux OS 
### Create Conda environment and install dependencies
```bash
conda create -n saas_hc python tqdm tabulate pyaml gxx cmake make mlpack cereal 
``` 
### Extract ThOP benchmark instances
```bash
tar -xzf instances/thop_instances.tar.gz -C instances/
```
### 
## Usage
### Activate Conda environment
```bash
conda activate saas_hc
```
### Run SAAS-HC experiment
```bash
cd src
python run_experiments.py --sol_dir ../experiments/saas_hc
```
## Acknowledgement
Our code is heavily inspired by [this repository](https://github.com/jonatasbcchagas/acoplusplus_thop) which is the source code of the paper ["Efficiently solving the thief orienteering problem with a max-min ant colony optimization algorithm"](https://link.springer.com/article/10.1007/s11590-021-01824-y) by Jonatas B. C. Chagas and Markus Wagner.

Besides that, we used the K-Means library from [mlpack](https://github.com/mlpack/mlpack) and the C implementation of the CMA-ES algorithm from [this repository](https://github.com/CMA-ES/c-cmaes) by Nikolaus Hansen.

This research was funded by University of Information Technology - Vietnam National University HoChiMinh City.

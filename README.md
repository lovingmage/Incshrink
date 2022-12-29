# IncShrink
![x86](https://github.com/emp-toolkit/emp-ag2pc/workflows/x86/badge.svg)


IncShrink is a secure outsourced database framework for dynamically growing data with view-based query answering. More details of the protocol can be found in the [paper](https://arxiv.org/abs/2203.05084).

# Compile
Switch to the file directory 

```console
  lovingmage@mac:~$ cmake .
  lovingmage@mac:~$ make
```
The binaries are compiled and stored in `./bin` folder

## Test

* Test filter query

 ```console
  lovingmage@mac:~$ python3 test_filter.py
```
* Test join query

```console
  lovingmage@mac:~$ python3 test_join.py
```
  
* Interface for costumize tests

```console
  lovingmage@mac:~$ python3 dispatcher.py
```

### Question
Please send email to cw374@duke.com

## Acknowledgement
This work is based on [EMP-Tool](https://github.com/emp-toolkit/emp-tool) project.

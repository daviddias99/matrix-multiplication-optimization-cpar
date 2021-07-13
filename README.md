# Performance of cache-aware matrix multiplication optimizations

**2020/2021** - 4th Year, 2nd Semester

**Course:** *Computação Paralela* ([CPAR](https://sigarra.up.pt/feup/en/UCURR_GERAL.FICHA_UC_VIEW?pv_ocorrencia_id=399883)) | Parallel Computing

**Authors:** David Silva ([daviddias99](https://github.com/daviddias99)), Luís Cunha ([luispcunha](https://github.com/luispcunha))

---

**Description:** For this project we studied the performance of matrix multiplication algorithms. In order to study cache-efficiency the algorithm was altered to take this concepts into account by switching loop orders and by implementing blocking versions of the algorithm. We studied the behaviour of the algorithms for different matrix and block-sizes as well as different languages (in this case C/C++ and Java). In the C/C++ version we utilized the PAPI library in order to evaluate cache-misses to further confirm the hypothesis.

For info on the proposed work on `docs/specification.pdf` and on our results in `docs/report.pdf`

**Technologies:** C/C++, Python, Java, PAPI library

**Skills:** Algorithms, cache efficiency, matrix multiplication, blocking approach

**Grade:** 17,7/20

---

## Instructions

To run the experiments use the command `python3 run_experiments.py`. Please ensure that you have the correct system permissions for using the PAPI library by running the `setup_papi.sh` script.
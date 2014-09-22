/*
 * Perform a fast fourier transform (real to complex or complex to real) of
 * a 1 dimensional raw data set and print the results on stdout
 *
 * Usage: rawfft [-i] -n count datafile
 *
 * Where:
 *	-i		Do inverse (complex to real - backward)
 *	-n count	Count of data values (input for forward, output for
 *			inverse/backward).
 *	datafile	name of file containing input data
 *
 * This program will output on stdout the result of the FFT. For a forward
 * transform (real to complex), it will output three columns of floating
 * point numbers:
 *
 *	Real		Imaginary		Magnitude
 *
 * In the inverse (backward) case, the program expects to parse a data file
 * containing those three columns and it will output the normalised real
 * values (which should match the original input signal).
 *
 * For example, imagine we have a raw data file of 256 lines, with one real
 * floating point number per line called "input_data" ...
 *
 * rawfft -n 256 input_data > fft_output
 *
 * 'fft_output' should contain the three column output described above (but
 * only 129 lines of data, i.e. input count / 2 + 1). If we then invert the
 * operation thus ...
 *
 * rawfft -i -n 256 fft_output > orig_data
 *
 * We should end up with input_data and orig_data being identical. NOTE that
 * the -n data count is THE ORIGINAL size
 *
 * Jimmo - www.jimmo.org
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fftw3.h>

/*
 * Load input data from a file. The format of the input file is determined
 * by the value of the 'inverse' argument (see comment at start of this file)
 */
void
load_data(int inverse, void *data, char *path, int ncount)
{
	double *dp;
	fftw_complex *cp;
	int k, eof;
	FILE *fp;
	double r, i, m;

	if (inverse)
		cp = (fftw_complex *)data;
	else
		dp = (double *)data;

	if ((fp = fopen(path, "r")) == NULL) {
		perror("cannot open input file");
		exit(-1);
	}

	/*
	 * Loop through, reading data up to the requested data count. If
	 * we hit EOF, we pad with zero
	 *
	 * Yeah, I could be a bit more solid with my input file format
	 * checking but this IS just an example :)
	 */
	for (eof = k = 0; k < ncount; k++) {
		if (inverse) {
			if (eof ||
			    fscanf(fp, "%lf %lf %lf\n", &r, &i, &m) == EOF) {
				eof = 1;
				r = i = 0;
			}
			cp[k][0] = r;
			cp[k][1] = i;
		} else {
			if (eof || fscanf(fp, "%lf\n", &r) == EOF) {
				eof = 1;
				r = 0;
			}
			dp[k] = r;
		}
	}

        fclose(fp);
}

/*
 * Loop through the resultant data, with a little processing where necessary,
 * and write it to stdout, again in a format as described in the opening
 * comments above.
 */
void
process_result(int inverse, void *data, int ncount)
{
	fftw_complex *cp;
	double *dp, r, i, mag;
	int k, max;

	if (inverse) {
		max = ncount;
		dp = (double *)data;
	} else {
		cp = (fftw_complex *)data;
		max = (ncount / 2) + 1;
	}

	for (k = 0; k < max; k++) {
		if (inverse) {
			/*
			 * important: normalise the data by dividing by the
			 * data count!
			 */
			r = (dp[k] / ncount);
			printf("%lf\n", r);
		} else {
			r = cp[k][0];
			i = cp[k][1];
			mag = sqrt((r * r) + (i * i));
			printf("%lf %lf %lf\n", r, i, mag);
		}

	}
}


int
main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	char *path;
	int opt, inverse = 0;
	int ncount = 0;
	double *dp;
	fftw_plan plan;
	fftw_complex *cp;

	while ((opt = getopt(argc, argv, "in:")) != -1) {
		switch (opt) {
		case 'i':
			inverse = 1;
			break;

		case 'n':
			ncount = atoi(optarg);
			break;

		default:
			fprintf(stderr, "usage: %s [-i] -n count input_file\n",
			    argv[0]);
			return (-1);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "%s: expected a file name argument!\n",
		    argv[0]);
		return (-1);
	}

	if (ncount <= 0) {
		fprintf(stderr, "%s: data count must be > 0\n", argv[0]);
		return (-1);
	}

	path = argv[optind];

	dp = (double *)malloc(sizeof (double) * ncount);
	cp = (fftw_complex *)fftw_malloc(sizeof (fftw_complex) * ncount);
	memset(dp, 0, sizeof (double) * ncount);
	memset(cp, 0, sizeof (fftw_complex) * ncount);

	if (inverse) {
		plan = fftw_plan_dft_c2r_1d(ncount, cp, dp, FFTW_MEASURE);
		load_data(inverse, cp, path, ncount);
	} else {
		plan = fftw_plan_dft_r2c_1d(ncount, dp, cp, FFTW_MEASURE);
		load_data(inverse, dp, path, ncount);
	}

	fftw_execute(plan);

	if (inverse)
		process_result(inverse, dp, ncount);
	else
		process_result(inverse, cp, ncount);

	fftw_destroy_plan(plan);

	return (0);
}

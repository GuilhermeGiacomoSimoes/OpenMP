#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<omp.h>
#include<assert.h>

#define VECTOR_SIZE 10

float float_rand(float min, float max);
void generating_vectors(float a, float b);
void print_vectors();

double v[VECTOR_SIZE];
int result[VECTOR_SIZE], quantidade[VECTOR_SIZE], v_trunc[VECTOR_SIZE], loop_numbers = 0, bins;

int main(int argc, char* argv[]) {
    //verificando se tem parametros passados pela linha de comando
    assert(argc > 1);
    //caso tenha pega esse parametro que sera o numero de threads
    const int thread_count = strtol(argv[1], NULL, 10);

    srand((unsigned int)time(NULL));

	float a=0.0, b=10.0;

	printf("nmr minimo a ser gerado: ");
	scanf("%f", &a);

	printf("nmr maximo a ser gerado: ");
	scanf("%f", &b);

	generating_vectors(a, b);

    #pragma omp parallel num_threads(thread_count)
	print_vectors();

	return 0;
}

float float_rand(float min, float max){
	float scale = rand() / (float) RAND_MAX;
	return min + scale * (max - min);
}

void generating_vectors(float a, float b){
	for(int i=0; i<VECTOR_SIZE; i++)
		v[i] = float_rand(a, b);

	//truncando os valores
	for(int i=0; i<VECTOR_SIZE; i++)
		v_trunc[i] = trunc(v[i]);

	//insertion sort
	int key;
	for(int i=1; i<VECTOR_SIZE; i++){
		key = v_trunc[i];
		int j = i -1;
		while(j >= 0 && v_trunc[j] > key){
			v_trunc[j + 1] = v_trunc[j];
			j = j-1;
		}

		v_trunc[j + 1] = key;
	}

	//contando numeros repetidos	
	for(int i=0; i<VECTOR_SIZE; i++)	
		quantidade[i] = 0;

	int x = 0;
	for(int i=0; i<VECTOR_SIZE; i++){
		i--;
		do{
			i++;
			quantidade[x]++;
			if((i + 1) == VECTOR_SIZE)	
				break;

		}while(v_trunc[i] == v_trunc[i + 1]);
		x++;
	}

	//criando vetor sem numeros repetidos
	for(int i=0; i<VECTOR_SIZE; i++)
		result[i] = -1;

	int j, m=0;
	for(int i=0; i<VECTOR_SIZE; i++){
		for(j = 0; j < m; j++)
			if(v_trunc[i] == result[j])
				break;
		if(j == m){
			result[m] = v_trunc[i];
			m++;
		}
	}

	int i=0;
	while(result[i] != -1){
		loop_numbers++;
		i++;
	}
}

void print_vectors(){
	const long my_rank = omp_get_thread_num();
    const int thread_count = omp_get_num_threads();

	if(my_rank <= loop_numbers){
		int end = (my_rank == thread_count - 1) ? loop_numbers : my_rank;
        int i;

        #pragma omp parallel for private(i)
		for(i=(my_rank - 1); i<end; i++){
			printf("%d ", result[i]);
			for(int j=0; j<quantidade[i]; j++)
				printf("#");
			printf("%d \n", quantidade[i]);
        }
	}
}
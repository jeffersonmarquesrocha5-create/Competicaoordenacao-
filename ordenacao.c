
static int32_t *lerArquivo(const char *caminho, size_t *total)
{
    FILE *f = fopen(caminho, "rb");
    if (!f) { perror("Erro ao abrir arquivo"); return NULL; }

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)tam + 1);
    if (!buf) { fclose(f); fprintf(stderr, "Sem memória\n"); return NULL; }
    size_t lidos = fread(buf, 1, (size_t)tam, f);
    fclose(f);
    buf[lidos] = '\0';

    
    size_t cap = lidos / 2 + 1, n = 0;
    int32_t *v = malloc(cap * sizeof(int32_t));
    if (!v) { free(buf); fprintf(stderr, "Sem memória\n"); return NULL; }

    char *p = buf, *fim = buf + lidos;
    while (p < fim) {
        
        while (p < fim && !(*p == '-' || (*p >= '0' && *p <= '9'))) p++;
        if (p >= fim) break;

        int neg = 0;
        if (*p == '-') { neg = 1; p++; }
        int64_t x = 0;
        while (p < fim && *p >= '0' && *p <= '9') x = x * 10 + (*p++ - '0');

        if (n == cap) {                       
            cap *= 2;
            int32_t *tmp = realloc(v, cap * sizeof(int32_t));
            if (!tmp) { free(v); free(buf); return NULL; }
            v = tmp;
        }
        v[n++] = (int32_t)(neg ? -x : x);
    }
    free(buf);
    *total = n;
    return v;
}


static int countingSort(int32_t *v, size_t n, int32_t min, int32_t max)
{
    size_t k = (size_t)((int64_t)max - min + 1);
    uint32_t *cont = calloc(k, sizeof(uint32_t));
    if (!cont) return 0;

    for (size_t i = 0; i < n; i++) cont[v[i] - min]++;

    size_t pos = 0;
    for (size_t j = 0; j < k; j++) {
        uint32_t c = cont[j];
        int32_t valor = (int32_t)(j + min);
        while (c--) v[pos++] = valor;
    }
    free(cont);
    return 1;
}


static int radixSort(int32_t *v, size_t n)
{
    uint32_t *a = (uint32_t *)v;
    uint32_t *b = malloc(n * sizeof(uint32_t));
    if (!b) return 0;

    size_t hist[4][256];
    memset(hist, 0, sizeof(hist));

    
    for (size_t i = 0; i < n; i++) {
        uint32_t x = a[i] ^ 0x80000000u;
        hist[0][x & 0xFF]++;
        hist[1][(x >> 8) & 0xFF]++;
        hist[2][(x >> 16) & 0xFF]++;
        hist[3][x >> 24]++;
    }

    uint32_t *src = a, *dst = b;
    for (int pass = 0; pass < 4; pass++) {
        size_t *h = hist[pass];
        if (h[(src[0] ^ 0x80000000u) >> (pass * 8) & 0xFF] == n) continue; 

        size_t soma = 0;                       
        for (int d = 0; d < 256; d++) { size_t t = h[d]; h[d] = soma; soma += t; }

        int sh = pass * 8;
        for (size_t i = 0; i < n; i++) {
            uint32_t x = src[i];
            dst[h[((x ^ 0x80000000u) >> sh) & 0xFF]++] = x;
        }
        uint32_t *t = src; src = dst; dst = t;
    }
    if (src != a) memcpy(a, src, n * sizeof(uint32_t));
    free(b);
    return 1;
}


static const char *algoritmoUsado = "";

static void ordenarVetor(int32_t *v, size_t n)
{
    if (n < 2) return;

    int32_t min = v[0], max = v[0];
    for (size_t i = 1; i < n; i++) {
        if (v[i] < min) min = v[i];
        if (v[i] > max) max = v[i];
    }
    uint64_t k = (uint64_t)((int64_t)max - min) + 1;

    
    if (k <= (uint64_t)n * 2 && countingSort(v, n, min, max)) {
        algoritmoUsado = "Counting Sort";
        return;
    }
    if (radixSort(v, n)) { algoritmoUsado = "Radix Sort LSD"; return; }

    fprintf(stderr, "Sem memória para ordenar\n");
    exit(1);
}


static int estaOrdenado(const int32_t *v, size_t n)
{
    for (size_t i = 1; i < n; i++)
        if (v[i - 1] > v[i]) return 0;
    return 1;
}

static int64_t somar(const int32_t *v, size_t n)
{
    int64_t s = 0;
    for (size_t i = 0; i < n; i++) s += v[i];
    return s;
}

int main(int argc, char **argv)
{
    const char *entrada = argc > 1 ? argv[1] : "ListahAhSerOrdenada.txt";

    size_t n = 0;
    int32_t *vetor = lerArquivo(entrada, &n);       
    if (!vetor) return 1;
    printf("Elementos lidos: %zu\n", n);

    int64_t somaAntes = somar(vetor, n);

    
    clock_t inicio = clock();
    ordenarVetor(vetor, n);
    clock_t fim = clock();
    

    double tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Algoritmo: %s\n", algoritmoUsado);
    printf("Tempo de ordenação: %f segundos (%f ms)\n", tempo_gasto, tempo_gasto * 1000.0);

    int ok = estaOrdenado(vetor, n) && somar(vetor, n) == somaAntes;
    printf("Verificação: %s (ordem crescente, %zu elementos, soma %lld)\n",
           ok ? "OK" : "FALHOU", n, (long long)somaAntes);
    if (n) printf("Menor: %d | Maior: %d\n", vetor[0], vetor[n - 1]);

    if (argc > 2) {                                  
        FILE *out = fopen(argv[2], "w");
        if (out) {
            for (size_t i = 0; i < n; i++) fprintf(out, "%d\n", vetor[i]);
            fclose(out);
            printf("Saída gravada em %s\n", argv[2]);
        }
    }

    free(vetor);
    return ok ? 0 : 1;
}

#include "./rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define EMPTY ""

char *strdup(const char *s);


// functie de creare a nodului unui Rope
RopeNode* makeRopeNode(const char* str) {
    // alocam memoria pentru nod
    RopeNode* rn = (RopeNode *)malloc(sizeof(RopeNode));

    // initializam copiii sai ca fiind NULL
    rn->left = NULL;
    rn->right = NULL;

    // greutatea nodului va fi lungimea stringului sau
    rn->weight = strlen(str);

    // asignarea stringului
    rn->str = str;

    return rn;
}

// functie de creare a unui RopeTree, cu radacina dorita data ca argument
RopeTree* makeRopeTree(RopeNode* root) {
    // alocam memoria pentru RopeTree
    RopeTree* rt = (RopeTree *)malloc(sizeof(RopeTree));

    // asignam tree-ului radacina dorita
    rt->root = root;

    return rt;
}

// functie de copiere a unui RopeNode
RopeNode* copy_RopeNode(RopeNode* rn) {
    if (!rn)
        return NULL;

    RopeNode* new_rn = makeRopeNode(strdup(rn->str));
    new_rn->left = copy_RopeNode(rn->left);
    new_rn->right = copy_RopeNode(rn->right);

    new_rn->weight = rn->weight;
    return new_rn;
}

// functie de copiere a unui RopeTree
RopeTree* copy_RopeTree(RopeTree* rt) {
    if (!rt)
        return NULL;

    RopeTree* new_rt = makeRopeTree(copy_RopeNode(rt->root));
    return new_rt;
}



void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}

int getTotalWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    return rt->weight + getTotalWeight(rt->right);
}

// functie de calcul a greutatii subarborelui stang al unui nod
int getLeftWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    RopeNode* lft = rt->left;

    return getTotalWeight(lft);
}

// functie de concatenare a 2 RopeTree-uri
RopeTree* concat(RopeTree* rt1, RopeTree* rt2) {
    RopeNode* new_root = makeRopeNode(EMPTY);

    // alocam string-ul noii radacini a RopeTree-ului
    new_root->str = strdup(EMPTY);

    // crearea de legaturi intre noua radacina si radacinile
    // celor 2 RopeTree-uri pe care le concatenam
    new_root->left = rt1->root;
    new_root->right = rt2->root;

    // calculam greutatea noii radacini
    new_root->weight = getLeftWeight(new_root);

    // construim noul RopeTree
    return makeRopeTree(new_root);
}

// functie recursiva de calcul pentru index
static char __indexRope(RopeNode* rn, int idx) {
    // daca root-ul nu are copii, inseamna ca caracterul dorit este
    // in stringul sau
	if (!rn->left && !rn->right) {
		return rn->str[idx];

	} else if (rn->weight > idx) {
        // daca indexul este mai mic decat greutatea, cautam caracterul
        // in subarborele stang
		return __indexRope(rn->left, idx);
	} else {
        // daca indexul este mai mare decat greutatea, cautam caracterul
        // in subarborele drept
		return __indexRope(rn->right, idx - rn->weight);
	}
}

// functia de calcul a indexului
char indexRope(RopeTree* rt, int idx) {
    if (!rt)
        return 0;
    // apelam functia recursiva pentru root
    return __indexRope(rt->root, idx);
}

// functia de cautare a unui substring
char* search(RopeTree* rt, int start, int end) {
    // ne alocam substring-ul, acel +1 fiind pentru delimitatorul '\0'
	char *str = malloc(sizeof(char) * (end - start + 1));
	int k = 0;

    // parcurgem caracterele dintre indecsii dati ca argument
	for (int i = start; i < end; i++) {
        // adaugam la substringul nostru, initial gol, caracterele intalnite
		str[k] = indexRope(rt, i);
        k++;
    }

    // adaugam la final delimitatorul
    str[k] = '\0';
    return str;
}

// functia recursiva de split
static void __splitRope(RopeNode* rn, int idx,
    RopeNode** eliminated_array, int *nr){
    if (strlen(rn->str) != 0) {
        // daca nodul este frunza
        if (idx == 0) {
            // daca caracterul de la care incepem split-ul este la inceputul
            // string-ului din nod, il adaugam la vectorul de noduri eliminate
            eliminated_array[(*nr)++] = rn;
            return;
        }

        // daca caracterul de la care incepem split-ul este in interiorul
        // string-ului din nod, spargem string-ul in 2 si facem 2 frunze cu
        // string-urile formate
        char *str1 = malloc(sizeof(char) * (idx + 2));
        char *str2 = malloc(sizeof(char) * (rn->weight - idx + 2));
        int k = 0;

        // formarea primului string, de la primul caracter pana la index
        for (int i = 0; i < idx; i++) {
            str1[k] = rn->str[i];
            k++;
        }

        str1[k] = '\0';

        k = 0;
        // formarea celui de-al doilea string, de la index pana la final
        for (int i = idx; i < rn->weight; i++) {
            str2[k] = rn->str[i];
            k++;
        }
        str2[k] = '\0';

        // crearea legaturilor intre nodul spart si cele 2 frunze formate
        rn->left = makeRopeNode(str1);
        rn->right = makeRopeNode(str2);

        // adaugarea frunzei din dreapta in array-ul de noduri eliminate
        eliminated_array[*nr] = rn->right;
        (*nr)++;
        // taiem legatura cu Tree-ul initial
        rn->right = NULL;

        free((void *)rn->str);
        // facem stringul din nodul spart sa fie gol
        rn->str = strdup(EMPTY);

        // recalculam greutatea
        rn->weight = getTotalWeight(rn->left);
        return;

    } else if (idx < rn->weight) {
        // daca splitul se face in subarborele stang

        // subarborele drept va fi taiat,
        // fiind adaugata radacina sa in array-ul de noduri eliminate
        eliminated_array[*nr] = rn->right;
        (*nr)++;

        // se taie legatura cu Tree-ul initial
        rn->right = NULL;

        // apelam recursiv functia pentru subarborele stang
        __splitRope(rn->left, idx, eliminated_array, nr);
        // recalculam greutatea
        rn->weight = getTotalWeight(rn->left);

    } else if (idx == rn->weight) {
        // in cazul in care indexul este egal cu greutatea nodului pe care
        // ne aflam, vom taia subarborele drept, adaugand radacina sa in
        // array-ul de noduri eliminate

        eliminated_array[*nr] = rn->right;
        (*nr)++;
        rn->right = NULL;
        return;

    } else {
        // cazul in care split-ul se face in subarborele drept

        // apelam recursiv functia pentru subarborele drept
        __splitRope(rn->right, idx - rn->weight, eliminated_array, nr);
    }
}

// functia de split
SplitPair split(RopeTree* rt, int idx) {
    // initializam rezultatul
    SplitPair split_rt;

    // facem o copie a tree-ului initial, intrucat tree-ul argument
    // nu poate fi modificat, pentru ca functia e apelata de mai multe ori
    RopeTree* new_rt = copy_RopeTree(rt);

    // alocarea unui array in care vom stoca Rope-Node-urile ale
    // caror legaturi cu Tree-ul initial le taiem
    RopeNode** eliminated_array = calloc(100, sizeof(RopeNode*));
    int nr_rn = 0;  // numarul de elemente din array

    if (idx == 0) {
        // cazul in care facem split de la primul caracter
        split_rt.left = makeRopeNode(strdup(EMPTY));
        split_rt.right = new_rt->root;

        free(new_rt);
        free(eliminated_array);
        return split_rt;

    } else if (idx >= getTotalWeight(new_rt->root)) {
            // cazul in care facem split pe ultimul
            // caracter sau index prea mare
        split_rt.left = new_rt->root;
        split_rt.right = makeRopeNode(strdup(EMPTY));

        free(new_rt);
        free(eliminated_array);
        return split_rt;

    } else {
        // apelam functia recursiva de split care ne va forma subarborele
        // stang si ne va popula subarborele drept in vederea construirii
        // subarborelui drept
        __splitRope(new_rt->root, idx, eliminated_array, &nr_rn);
    }

    if (nr_rn == 1) {
        // daca avem un singur element in array-ul de noduri
        // nu e nevoie sa concatenam nimic
        split_rt.left = new_rt->root;
        split_rt.right = eliminated_array[0];

        free(new_rt);
        free(eliminated_array);
        return split_rt;

    } else {
        // daca avem mai multe elemente in array este nevoie sa
        // concatenam toate elementele array-ului intr-un RopeTree
        // prin concatenarea succesiva a elementelor din array 2 cate 2

        RopeNode* rn1 = eliminated_array[nr_rn - 1];
        // ultimul nod "rupt" de tree

        for (int i = nr_rn - 2; i >= 0; i--) {
            // parcurgerea nodurilor rupte de la cel mai
            // nou adaugat la cel mai vechi
            RopeNode* rn2 = eliminated_array[i];

            RopeTree* rt1 = makeRopeTree(rn1);
            RopeTree* rt2 = makeRopeTree(rn2);

            // concatenarea 2 cate 2
            RopeTree *big_tree = concat(rt1, rt2);
            rn1 = big_tree->root;

            free(rt1);
            free(rt2);
            free(big_tree);
        }

        // nodul stang in urma split-ului va fi radacina tree-ului
        split_rt.left = new_rt->root;

        // nodul drept in urma split-ului va fi radacina tree-ului
        // format in urma concatenarii
        split_rt.right = rn1;

        free(new_rt);
        free(eliminated_array);
        return split_rt;
    }
}

RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    // se sparge string-ul in locul unde vrem sa inseram
    SplitPair split_rt = split(rt, idx);

    // facem un nod care sa contina string-ul pe care il adaugam
    RopeNode* new_node = makeRopeNode(str);
    // construim un tree care sa aiba ca radacina nodul creat anterior
    RopeTree* middle_tree = makeRopeTree(new_node);

    // construim un tree care sa aiba ca radacina nodul stang
    // rezultat al split-ului
    RopeTree* left_tree = makeRopeTree(split_rt.left);
    // construim un tree care sa aiba ca radacina nodul drept
    // rezultat al split-ului
    RopeTree* right_tree = makeRopeTree(split_rt.right);

    // concatenarea string-ului stang cu string-ul pe care il adaugam
    RopeTree* big_left_tree = concat(left_tree, middle_tree);
    // concatenarea finala care ne definitiveaza inserarea
    RopeTree* big_tree = concat(big_left_tree, right_tree);

    free(left_tree);
    free(middle_tree);
    free(right_tree);
    free(big_left_tree);
    return big_tree;
}

RopeTree* delete(RopeTree* rt, int start, int len) {
    // spargem RopeTree-ul astfel incat, atunci cand concatenam
	// bucatile special alese, stergem substringul dorit din rope
    SplitPair left_split_rt = split(rt, start);
    SplitPair right_split_rt = split(rt, start + len);

    RopeTree* left_tree = makeRopeTree(left_split_rt.left);
    RopeTree* right_tree = makeRopeTree(right_split_rt.right);

    RopeTree* big_tree = concat(left_tree, right_tree);

    free(left_tree);
    free(right_tree);

    return big_tree;
}

// FINAL 10p -> complex test involving all operations

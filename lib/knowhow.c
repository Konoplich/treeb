#ifndef knowhow_c
#define knowhow_c
#include <interface.c>
#include <public.c>
#include <protected.c>
#include <private.c>
#include <stdlib.h>

u_int32_t deleteValFromNode(TREE, u_int32_t val, ROOT); 

// 검색하는 함수
void* searchNode(const TREE, ROOT, u_int32_t val){
    if (!p_root){
        //printf("Empty tree.\n");
        return 0;
    }
    node_t* level = p_root;
    while (true){
        u_int32_t pos;
        for (pos=0; pos<level->cnt_key; pos++){
            if (val== level->key[pos].key){
                //printf("key %d exists!",val);
                return level->key[pos].p_context;
            }
            else if (val<level->key[pos].key){
                break;
            }
        }
        if (level->leaf) break;
        level = level->child[pos];
    }
    //printf("key %d does not exist",val);
    return NULL;
}


// Функции, которые разделяют значение узла и распределяют его между другими узлами
node_t* splitNode(TREE, u_int32_t pos, ROOT, node_t* parent){ 
    p("현재 노드, 부모 노드, 현재노드에서 넣은 값의 위치를 알고있는 pos를 인자로 받기\n");
    u_int32_t median; // 분리를 위해 중앙값 알아야 함.
    median = p_root->cnt_key / 2; // Медиана равна 'keycount/2' для четных или нечетных чисел
    node_t* right_p_root; // Создайте узел справа, в который мы поместим новое разделенное значение (которое впоследствии станет дочерним)

    right_p_root = (node_t*)malloc(sizeof(node_t));
    right_p_root->child = (node_t**)malloc((p_tree->max_children+1)*sizeof(node_t*));
    //memset(right_p_root->child, 0, (p_tree->max_children+1)*sizeof(node_t*));
    right_p_root->key = (void_ref_t*)malloc((p_tree->max_keys+1)*sizeof(void_ref_t));
    //memset(right_p_root->key, 0, (p_tree->max_keys+1)*sizeof(void_ref_t));
    right_p_root-> leaf = p_root->leaf; 
    // Он только отделен, так что это тот же самый лист или нет, что и отделяемый узел. Если он изначально не был листом, то он и не лист.
    p("분리만 한거니까 현재 분리하려는 노드와 리프 여부가 같음. 걔가 원래 리프 아니였으면 얘도 리프 아님.\n");
    right_p_root-> cnt_key = 0;
    right_p_root-> cnt_child = 0;
   
    u_int32_t num_iter = p_root->cnt_key;
    p("u_int32_t num_iter = %d;\n", p_root->cnt_key);
    for(u_int32_t i = median+1; i< num_iter; i++){ // Добавьте ключ к узлу, который вы хотите отсоединить (независимо от того, является ли он листом или нет, с помощью
        p("분리할 노드에 키 담기(리프이든 아니든\n")
        right_p_root->key[i-(median+1)] = p_root->key[i];
        right_p_root ->cnt_key++;
        p_root->cnt_key--;
    }

    if (!p_root->leaf){ 
        p("median %d\n", median);
        p("현재 노드가 리프가 아니면, 자식 담기\n");
        num_iter = p_root->cnt_child;
        p("%d %d\n", median+1, num_iter);
        for (u_int32_t i = median+1; i < num_iter; i++ ) { // Если текущий узел не является листом, добавьте дочерние узлы
            p("오른쪽 노드에 현재 노드 자식 절반 담기\n");
            right_p_root->child[i-(median+1)] = p_root ->child[i];
            p("@\n");
            right_p_root-> cnt_child++; 
            p("@@\n");
            p("새로 채워준 노드의 자식 개수는 증가, 현재 노드에서는 빼기\n");
            p_root->cnt_child--;
            p("@@@\n");
        }
        p("exit not leaf\n");
    }
    // Обработка родительских узлов для перемещения вверх при отсоединении
    p("분리할 때 위로 올릴 부모 노드 처리\n");
    if (p_root==p_tree->data) { // Если корень, то должен быть создан новый родительский узел
        p("루트면 새 부모 노드 만들어야 됨.\n");
        node_t* new_parent_p_root;
        new_parent_p_root = create_node(p_tree, p_root->key[median]); // Создайте новый родительский узел с медианой
        p("중앙값 가지고 새 부모 노드 만들기\n");
        p_root->cnt_key--;
        new_parent_p_root->child[0] = p_root; // Левым дочерним узлом нового родительского узла является текущий узел
        p("새부모노드의 왼쪽 자식은 현재 노드\n");
        new_parent_p_root->child[1] = right_p_root;

        new_parent_p_root->cnt_child = 2;
        return new_parent_p_root; 
    }
    else { // Если не корень, используйте исходный родительский узел
        p("루트가 아니면, 원래 있던 부모노드 활용\n");
        for (u_int32_t i= parent->cnt_key; i> pos; i--){ // Нам нужно поместить его в родительский узел, поэтому измените расположение ключей в нем
            p("부모 노드에 넣어야되니까 거기있던 키 배치 다시하기\n");
            parent->key[i] = parent->key[i-1];
            parent->child[i+1] = parent->child[i];
        }

        parent->key[pos]= p_root->key[median]; // Помещайте значения туда, где они должны быть в родительском узле
        parent->cnt_key++; // Добавьте больше ключей к родительскому узлу и уменьшите количество ключей на исходном узле
        p_root->cnt_key--;
        parent-> child[pos+1] = right_p_root; //Левый узел уже подключен, поэтому к родителю подключается только правая
                                              //сторона.
        parent->cnt_child+=1;
    }
    return p_root; //Вернуть текущий узел
}

node_t* insertNode(TREE, u_int32_t parent_pos, void_ref_t val, node_t* node, node_t* parent) { 
    p("\n\n삽입할 값,  \n");
    u_int32_t pos; // Должна иметь позицию ключа в текущем узле. Это необходимо, потому что нам нужно найти позицию значения, которое мы хотим вставить. 
    p("현재 노드에서 키의 위치를 갖고 있어야 함. 왜냐면 넣으려고 하는 값의 위치를 찾아야 하기 때문.\n");
    for (pos =0; pos < node->cnt_key; pos++ ) {
        p("Позиция POS, начиная с 0 и обходя количество ключей текущего узла\n");
        if (val.key == node -> key[pos].key){ // Если posth-ключ и val узла одинаковы, то
            p("node의 pos번째 키와 val이 같으면\n");
            //printf("Duplicates are not permitted!\n");
            return node;
        }
        else if (val.key < node->key[pos].key){ // Если val меньше, чем ключ posth узла, он останавливается на этой позиции.
            p("val이 node의 pos번째 키보다 작으면 그 pos에서 멈춘다.\n");
            break;
        }
    }// 만약 val이 그 node에 있는 값보다 크면 당연히 마지막 pos가 나올 것임.
     // Если val больше, чем значение на этом узле, то, конечно, выйдет последний pos.
    if (!node->leaf) { 
        p("node leaf 여부가 false이면, leaf가 아니면\n");
        node -> child[pos] = insertNode(p_tree, pos, val, node->child[pos] ,node); 
        p("node의 pos번째 자식 노드에 insertNode 값을 담는다. 재귀로 자식을 탐색하기 위해 또 들어감.\n");
        p("%d %d\n", node->cnt_key,p_tree->max_keys +1); 
        if (node->cnt_key == p_tree->max_keys +1){ // Если текущее количество ключей узлов может отклониться от правил, воспользуйтесь командой
            p("현재 노드 키 개수가 규칙에서 벗어날거같으면\n");
            node = splitNode(p_tree, parent_pos, node,parent); // Должны быть разделены в направлении вверх.
            p("윗 방향으로 분리를 해야 함.\n");
        }
    }
    else {// Логика вставки, когда лист 
        p("leaf일 때의 삽입 로직\n");
        p("for (u_int32_t i = %d; i > %d; i--)\n",node->cnt_key, pos); 
        for (u_int32_t i = node->cnt_key; i > pos; i--) { // Начиная с конца, пройдите по узлам назад до узла, в который нужно вставить val.
            p("끝에서부터 val을 삽입해야 하는 위치에 있는 노드까지의 노드들을 뒤로 땡기는 작업을 한다.\n");
            node -> key[i] = node->key[i-1]; // Ключ перемещается назад на один пробел
            node -> child[i+1] = node->child[i]; // То же самое касается и детей.
        }

        node -> key[pos] = val; // Вставьте val туда, куда его следует вставить.
        node -> cnt_key++; //Увеличьте количество ключей теперь, когда у вас есть один.
        if (node-> cnt_key == p_tree->max_keys+1){ // Если листовой узел переполнен, отсоедините его. 
            p("leaf 노드가 꽉 찼으면 분리를 해준다.\n");
            node = splitNode(p_tree, parent_pos, node,parent);
        }
    }  
    p("curr key %d child %d\n", node->cnt_key, node->cnt_child);
    return node; // Мы поместили значение в узел, поэтому нам нужно вернуть этот узел. Таким образом, рекурсия завершится, а значение будет доступно

}

// 삽입 함수 제작 (인자 : 받아야 하는 값)
void insert_rec(TREE, void_ref_t val){
    if (!p_tree->data){ // root가 없으면
        p_tree->data = create_node(p_tree, val); // root를 만들어라.
        p_tree->data -> leaf = true ; // 처음 만들어지는거니까 root이자 leaf 노드.
        return;
    }
    else{ // 루트가 있으면
        p_tree->data = insertNode(p_tree, 0,val,p_tree->data,p_tree->data); // 처음에는 root가 부모이자 리프노드.
    }
}

// 못빌릴 때 합치는 함수
void mergeNode(node_t* par_p_root, u_int32_t p_root_pos, u_int32_t mer_p_root_pos){
    // 왼쪽 노드를 지웠을 때에는 최종 merge되는 주체가 왼쪽 노드가 되게 강제로 만듬.(편리를 위해) / p_root_pos가 삭제된 키를 갖고있는 노드가 될 수도 있고 안될수도 있음.
    u_int32_t merge_idx = par_p_root->child[mer_p_root_pos]->cnt_key; // merge의 주체 노드쪽에 merge될 키 위치를 지정한다.
    par_p_root->child[mer_p_root_pos]->key[merge_idx]= par_p_root->key[mer_p_root_pos]; //부모노드의 키를 merge함.
    par_p_root->child[mer_p_root_pos]->cnt_key++;

    for(u_int32_t i=0; i<par_p_root->child[p_root_pos]->cnt_key; i++){ // 지우는 노드에서 그 키를 지워서 최소 키 개수 유지가 안될 수 있음. 남은 키 들은 merge한 노드로 옮겨야 함. 남은키가 없으면 아예 for문이 안돌아감.
        par_p_root->child[mer_p_root_pos]->key[merge_idx+1+i] = par_p_root->child[p_root_pos]->key[i]; // 키 개수가 2개 -> i가 0, merge idx 오른쪽 키/ 키 개수가 늘어나면 i도 하나씩 더 늘어남. 
        par_p_root->child[mer_p_root_pos]->cnt_key++;
    }

    u_int32_t merge_childidx = par_p_root->child[mer_p_root_pos]->cnt_child; //merge한 노드 끝부분으로 옮겨야 하니 끝부분 idx 지정.
    for (u_int32_t i=0; i<par_p_root->child[p_root_pos]->cnt_child; i++){ // 지우는 노드에서 키를 지우고 남은 자식이 있으니. 걔네를 merge한 노드로 옮겨야 한다.
        par_p_root->child[mer_p_root_pos]->child[merge_childidx+i] = par_p_root->child[p_root_pos]->child[i];
        par_p_root->child[mer_p_root_pos]->cnt_child++;
    }

    free(par_p_root->child[p_root_pos]->key);
    par_p_root->child[p_root_pos]->key = NULL;
    free(par_p_root->child[p_root_pos]->child);
    par_p_root->child[p_root_pos]->child = NULL;
    free(par_p_root->child[p_root_pos]);
    par_p_root->child[p_root_pos] = NULL;


    for(u_int32_t i = mer_p_root_pos; i < (par_p_root->cnt_key)-1; i++) { //부모 노드의 키 하나는 이미 자식 노드와 병합되었으니 재정비해야 함.
        par_p_root->key[i] = par_p_root->key[i+1];
    }
    par_p_root->cnt_key--;

    for(u_int32_t i = mer_p_root_pos+1; i<(par_p_root->cnt_child)-1; i++){ // 부모 노드에는 병합 한 거 말고 그 뒤에 다른 자식도 있을 수 있으니 재배열 해야 함. merge한 노드 뒷 노드부터 대상이 됨.
        par_p_root->child[i] = par_p_root->child[i+1];
    }
    par_p_root->cnt_child--;
}

// 왼쪽에서 빌리는 함수
void borrowFromLeft(node_t* par_p_root, u_int32_t cur_p_root_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. cnt_key는 최소상태 혹은 미만일거임.
    u_int32_t tenant_idx = 0; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.

    // 빌리는 노드는 자리를 마련해야 하기 때문에 남아있는거를 한칸 씩 뒤로 미뤄야 함.(남아있는게 없어도 적용). borrowFromRight와 다르게 먼저 자리 정리를 해야 함.
    for (u_int32_t i=0; i< par_p_root->child[cur_p_root_pos]->cnt_key; i++){ 
        par_p_root->child[cur_p_root_pos]->key[i+1] = par_p_root->child[cur_p_root_pos]->key[i];
    }
    par_p_root->child[cur_p_root_pos]->key[tenant_idx] = par_p_root->key[cur_p_root_pos-1]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 여기서는 왼쪽에서 빌려오니까, cur_p_root_pos(오른쪽 자식 가리키는 위치)에서 1을 빼야 부모의 키 위치가 됨. 그리고 형제노드의 키가 위로 올라가는 꼴.
    par_p_root->child[cur_p_root_pos]->cnt_key++;

    u_int32_t idx_from_sib_topar= (par_p_root->child[cur_p_root_pos-1]->cnt_key) -1; // 부모노드 키를 밑으로 내렸으니까 형제 노드 중 마지막 키를 하나 부모로 올려야 됨. 
    par_p_root->key[cur_p_root_pos-1] = par_p_root->child[cur_p_root_pos-1]->key[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다. 여기선 왼쪽 형제노드.
    par_p_root->child[cur_p_root_pos-1]->cnt_key--;

    // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
    if (par_p_root->child[cur_p_root_pos-1]->cnt_child>0){ // 형제 노드 자식이 있는 경우에만
        u_int32_t tenant_childidx = (par_p_root->child[cur_p_root_pos-1]->cnt_child)-1; // tenant 노드에다가 형제노드의 가장 뒷 자식 위치를 줘야되기 때문에 그 인덱스 지정.
        // 자식 위치 정리. borrowFromRight와 다르게 옮기기 전 미리 세팅해야 함. 
        for (u_int32_t i = par_p_root->child[cur_p_root_pos]->cnt_child; i > 0; i--){ // 뒤에서부터 시작해서 앞에껄 뒤로 옮기는 느낌.
            par_p_root->child[cur_p_root_pos]->child[i] = par_p_root->child[cur_p_root_pos]->child[i-1];
        }

        par_p_root->child[cur_p_root_pos]->child[0] = par_p_root->child[cur_p_root_pos-1]->child[tenant_childidx]; // 형제 노드 자식 빌려옴. 빌려온 자식 놓는 위치는 현재 노드의 1번째 위치.
        par_p_root->child[cur_p_root_pos]->cnt_child++;

        par_p_root->child[cur_p_root_pos-1]->cnt_child--;
    }
}

// 오른쪽에서 빌리는 함수
void borrowFromRight(node_t* par_p_root, u_int32_t cur_p_root_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. cnt_key는 최소상태 혹은 미만일거임.
    u_int32_t tenant_idx = par_p_root->child[cur_p_root_pos]->cnt_key; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.
    par_p_root->child[cur_p_root_pos]->key[tenant_idx] = par_p_root->key[cur_p_root_pos]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 그리고 형제노드의 키가 위로 올라가는 꼴.
    par_p_root->child[cur_p_root_pos]->cnt_key++;

    u_int32_t idx_from_sib_topar= 0;
    par_p_root->key[cur_p_root_pos] = par_p_root->child[cur_p_root_pos+1]->key[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다.

    // // 형제노드는 키를 빌려줬으니까 위치 정리를 해야 한다.(한 칸씩 앞으로 땡기기)
    for (u_int32_t i=0; i< (par_p_root->child[cur_p_root_pos+1]->cnt_key)-1; i++){ 
        par_p_root->child[cur_p_root_pos+1]->key[i] = par_p_root->child[cur_p_root_pos+1]->key[i+1];
    }
    par_p_root->child[cur_p_root_pos+1]->cnt_key--;

    u_int32_t idx_from_sib = 0;
    // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
    if (par_p_root->child[cur_p_root_pos+1]->cnt_child>0){ // 자식이 있는 경우에만
        u_int32_t tenant_childidx = par_p_root->child[cur_p_root_pos]->cnt_child; // tenant 노드에 빌려온 자식이 들어갈 위치
        par_p_root->child[cur_p_root_pos]->child[tenant_childidx] = par_p_root->child[cur_p_root_pos+1]->child[idx_from_sib]; // 형제 노드 자식 빌려옴.
        par_p_root->child[cur_p_root_pos]->cnt_child++;

        // 자식 위치 정리 
        for (u_int32_t i = 0; i<par_p_root->child[cur_p_root_pos+1]->cnt_child-1; i++){
            par_p_root->child[cur_p_root_pos+1]->child[i] = par_p_root->child[cur_p_root_pos+1]->child[i+1];
        }
        par_p_root->child[cur_p_root_pos+1]->cnt_child--;
    }
}

void balanceNode(TREE, ROOT, u_int32_t child_pos){ // 현재 노드와 자식노드에서의 위치를 인자로 받는 함수(빌리기,병합을 진행)
    if (child_pos==0){ // 자식노드 키 위치가 맨 왼쪽일때는 오른쪽 부모,형제를 봐야 함.
        if (p_root->child[child_pos+1]->cnt_key > p_tree->min_keys){ // (자식노드 기준) 형제의 키개수가 최소숫자 범위 안 부서질때
            borrowFromRight(p_root,child_pos);
        }
        else{ // 형제의 키개수가 최소숫자 범위 부서질때
            mergeNode(p_root,child_pos+1,child_pos); // 부모노드(현재노드)와 자신 위치랑 자기 형제 위치를 같이 넘겨줌.  
        }
        return;
    }

    else if (child_pos == (p_root->cnt_key)){ // 자식노드 키 위치가 맨 오른쪽일 때는 왼쪽 부모, 형제 봐야 함.
        if (p_root->child[child_pos-1]->cnt_key > p_tree->min_keys){ // 자식노드 기준, 왼쪽 형제의 키개수가 최소숫자 범위 안 부서질 때
            borrowFromLeft(p_root,child_pos);
        }
        else{ // 최소숫자 범위 부서질 때
            mergeNode(p_root,child_pos,child_pos-1); // 부모노드(현재노드)와 지우는 노드랑 병합대상 노드 위치를 같이 넘겨줌.  
        }
        return;
    }
    else{ // 맨 왼쪽,맨 오른쪽 말고 그 이외
        if (p_root->child[child_pos-1]->cnt_key > p_tree->min_keys){
            borrowFromLeft(p_root,child_pos); 
        }
        else if (p_root->child[child_pos+1]->cnt_key > p_tree->min_keys){
            borrowFromRight(p_root,child_pos);
        }
        else{
            mergeNode(p_root,child_pos,child_pos-1); // 극단에 있는 자식 말고 그 외 지역에 위치한 노드들이 병합할 때
        }
        return;
    }
}

// 내부노드 기준으로 자식들을 merge해야하는 케이스
u_int32_t mergeChildNode(node_t* par_p_root, u_int32_t cur_p_root_pos){
    u_int32_t mergeidx = par_p_root->child[cur_p_root_pos]->cnt_key; // merge는 왼쪽 기준으로 하는데 자식노드에서 합쳐질 위치 지정.
    // 바로 지우지 않고 합치려고 하는 노드에 지우려고 하는 부모노드(내부노드)의 값을 합침. 왜냐? 안 내리고 바로 지우고 자식노드만 합치면, 합치려고 하는 노드 밑에 또 자식노드가 있을 경우에는 자식 1개가 떠버리게 됨. 
    // 그래서 일단 부모노드의 값을 넣고 거기서 또 재귀로 들어가서 그 자식을 합치던가 빌리던가 해서 자식수를 해결해야 함.
    u_int32_t val_par_p_root = par_p_root->key[cur_p_root_pos].key; // 지우려는 부모 노드의 값을 기억
    par_p_root->child[cur_p_root_pos]->key[mergeidx] = par_p_root->key[cur_p_root_pos]; 
    par_p_root->child[cur_p_root_pos]->cnt_key++;

    // 합치려는 노드에 형제 노드 값을 가지고 옴. (원래 함수의 목적)
    for (u_int32_t i=0; i < par_p_root->child[cur_p_root_pos+1]->cnt_key; i++){
        par_p_root->child[cur_p_root_pos]->key[mergeidx+1+i] = par_p_root->child[cur_p_root_pos+1]->key[i];
        par_p_root->child[cur_p_root_pos]->cnt_key++;
    }
    // 형제노드 자식도 들고와야 함.
    for (u_int32_t i=0; i< par_p_root->child[cur_p_root_pos+1]->cnt_child; i++){
        par_p_root->child[cur_p_root_pos]->child[mergeidx+1+i] = par_p_root->child[cur_p_root_pos+1]->child[i];
        par_p_root->child[cur_p_root_pos]->cnt_child++;
    }

    // 부모노드(내부노드)의 키를 줬으니까 재배열 & 자식도 재배열
    for (u_int32_t i= cur_p_root_pos; i<par_p_root->cnt_key; i++){
        par_p_root->key[i] = par_p_root->key[i+1];
        par_p_root->cnt_key--;
    }
    for (u_int32_t i = cur_p_root_pos+1; i < par_p_root->cnt_child; i++){
        par_p_root->child[i]= par_p_root->child[i+1];
        par_p_root->cnt_child--;
    }
    return val_par_p_root; // 부모노드에서 내렸던 값을 지우기 위해 일단 리턴. 값을 삭제하는 함수에서 지워질 예정.

}

// predecessor 찾는 함수
// Найдите функции-предшественники
void_ref_t findPredecessor(node_t* cur_p_root){
    u_int32_t predecessor;
    if (cur_p_root->leaf){ //Если текущий навигационный узел является листом, он будет найден
        return cur_p_root->key[cur_p_root->cnt_key-1]; //Просто укажите наибольший ключ на текущем узле (условие предшественника)
    }
    return findPredecessor(cur_p_root->child[(cur_p_root->cnt_child)-1]);// При каждой навигации необходимо переходить к более крупному ребенку.
}
// predecessor 찾아서 내부노드에 덮어씌우는 함수
// Функции для поиска предшественников и перезаписи внутренних узлов
u_int32_t overrideWithPredecessor(node_t* par_p_root, u_int32_t pos_std_search){
    void_ref_t predecessor = findPredecessor(par_p_root->child[pos_std_search]); //Вызовите функцию, которая рекурсивно обходит предшественника. > Zoom с родительским узлом и позицией для движения вниз в качестве аргументов.
    par_p_root->key[pos_std_search] = predecessor; //Заменяет значение внутреннего узла, который вы хотите очистить, на найденного предшественника
    return predecessor.key;
}

// suceessor 찾는 함수
void_ref_t findSuccessor(node_t* cur_p_root){
    void_ref_t successor;
    if (cur_p_root->leaf){ //현재 탐색노드가 리프이면, 찾을 수 있음.
        return cur_p_root->key[0]; //현재 노드에서 가장 작은 키 주면 됨.(successor 조건)
    }
    return findSuccessor(cur_p_root->child[0]); // 탐색할 때마다 작은 쪽 자식으로 탐색해야 함.
}
// successor 찾아서 내부노드에 덮어씌우는 함수
u_int32_t overrideWithSuccessor(node_t* par_p_root, u_int32_t pos_std_search){
    void_ref_t successor = findSuccessor(par_p_root->child[pos_std_search+1]); // successor를 재귀로 쭉 내려가서 찾는 함수 호출. 부모 노드랑 타고 내려갈 위치(오른쪽으로 가야됨.)를 인자로 줌.
    par_p_root->key[pos_std_search] = successor; // 지우려고 하는 내부노드의 값에 찾은 successor로 대체 해줌.
    return successor.key;
}

// 내부 노드에서 값을 지우는 함수
void deleteInnerNode(TREE, node_t* cur_p_root, u_int32_t cur_p_root_pos){
    u_int32_t cessor = 0; // predecessor 혹은 successor가 있을 경우 || merge할 경우의 찾은 값을 담은 변수
    u_int32_t deletion_for_merge = 0;
    // 왼쪽 오른쪽 중 어느쪽 자식이 더 많은지 확인, prede혹은 successor를 찾아야 하기때문. 만약 같으면 무조건 왼쪽 보게 강제.
    if (cur_p_root->child[cur_p_root_pos]->cnt_key >= cur_p_root->child[cur_p_root_pos+1]->cnt_key){ 
        if(cur_p_root->child[cur_p_root_pos]->cnt_key > p_tree->min_keys){ // 자식 키개수가 최소범위 부시지 않으면 predecessor 찾기 가능.
            cessor = overrideWithPredecessor(cur_p_root,cur_p_root_pos);
            deleteValFromNode(p_tree, cessor,cur_p_root->child[cur_p_root_pos]); // 찾은 predecessor를 위로 올려야 함. 근데 이 과정이 결국 해당 리프노드에서 값을 지우는게 효과라서 삭제하는 함수 호출.
        }
        else{
            deletion_for_merge = mergeChildNode(cur_p_root,cur_p_root_pos);
            deleteValFromNode(p_tree, deletion_for_merge, cur_p_root->child[cur_p_root_pos]);
        }
    }
    else {
        if (cur_p_root->child[cur_p_root_pos+1]->cnt_key > p_tree->min_keys){
            cessor = overrideWithSuccessor(cur_p_root,cur_p_root_pos);
            deleteValFromNode(p_tree, cessor,cur_p_root->child[cur_p_root_pos+1]); // successor 찾으면 이것도 리프노드에서 지우는 효과를 내야 함.
        }
        else{
            deletion_for_merge = mergeChildNode(cur_p_root,cur_p_root_pos);
            deleteValFromNode(p_tree, deletion_for_merge, cur_p_root->child[cur_p_root_pos]);
        }

    }

}

// 노드랑 지우는 값을 넣어주면 지우는 함수.
u_int32_t deleteValFromNode(TREE, u_int32_t val, ROOT){ 
    u_int32_t pos; // 지우려는 노드의 포지션 위치를 잡음.    
    u_int32_t flag = false; // 지워졌는지 여부를 알기 위해 flag를 접수
    for (pos=0; pos < p_root->cnt_key; pos++){ // val이 지워져야하니 그 위치를 찾아야 함.현재 노드의 키 개수만큼 탐색
        if (val == p_root->key[pos].key){ // 현재 노드의 키 배열에서 pos와 val이 같으면
            flag = true; // 찾았다는 표시
            break;
        }
        else if (val< p_root->key[pos].key){ // 키 배열의 pos 위치 값이 val보다 크면 그 위치에서 멈춰라. 거기에서 아래로 더 들어가야 한다.
            break;
        }
    } // 이게 끝났다는건 그 노드에서 (추가 탐색할) pos위치가 정해졌다는 것 
    if (flag){ // flag가 true이면 실제로 삭제하는 작업 실시
        if (p_root->leaf){  // 리프에서 삭제해야 하면
            for (u_int32_t i = pos; i<p_root->cnt_key; i++){ // 저장된 pos위치부터 키 개수만큼 탐색
                p_root->key[i] = p_root->key[i+1]; // 지우려는 키 위치에 그 다음 키로 덮어쓰기
            }
            p_root->cnt_key--;
        }
        else { // 내부에서 삭제해야 하면
                deleteInnerNode(p_tree, p_root,pos); //내부 노드의 값을 삭제하는 함수 제작. 현재 노드와 현재노드에서의 값 위치를 인자로 넘김.
        }
        return flag;
    }
    else { // flag가 false이면(지우려는 값을 못찾은 것)
        if (p_root->leaf) { //leaf 노드이면
            return flag;
        }
        else{ // 지우려는 값을 못 찾았는데 내부 노드이면 더 내려감.
            flag = deleteValFromNode(p_tree, val, p_root->child[pos]); //val이랑 현재노드의 pos번째 자식 넘겨서 flag 받기
        }
    }
    if (p_root->child[pos]->cnt_key < p_tree->min_keys){ // (재귀가 끝나서 다시 올라온뒤)삭제처리했던 자식 노드의 키 개수가 최소숫자 범위 부셔졌을 때
        balanceNode(p_tree, p_root,pos); // 빌리던, 병합하던 하는 함수 제작 (현재 노드와 자식노드의 pos위치를 인자로)
    }

    return flag;    
}

// 지우는 함수
void delete_rec(TREE, ROOT, u_int32_t val){ // 현재 노드랑 지우려는 값을 인자로 받음.
    if (!p_root){ // 현재 보는 노드에 아예 값이 없으면
        //printf("Empty tree!!\n");
        return;
    }
    u_int32_t flag = deleteValFromNode(p_tree, val,p_root); // 현재 노드 내에서 값을 지우는 함수 호출. 지우는 값이랑 현재 노드를 인자로 받음. 리턴은 flag로 받음.
    if (!flag){ // flag가 0이면 실행.
        //printf("%d does not exist in this tree. \n", val); // 함수 내에서 못찾으면 플래그가 0인거니까 에러 메세지 출력
        return;
    }
    if (p_root->cnt_key == 0){  // deleteVal을 하고 나서 p_root의 키개수가 0일 때 = 현재 노드에 아무것도 없어서 변화가 필요함.
        p_root = p_root->child[0]; // 지금 노드를 가장 왼쪽 자식 노드로 만듬.
    }
    p_tree->data = p_root;

}

bool insert(TREE, void_ref_t ref)
{
    if(!p_tree)
        return false;
    insert_rec(p_tree, ref);
}
void delete(TREE, const u_int32_t key)
{
    if(!p_tree || !p_tree->data)
        return;
    delete_rec(p_tree, p_tree->data, key);
}

void* lookup(TREE, const u_int32_t key)
{
    if(!p_tree || !p_tree->data)
        return NULL;
    return searchNode(p_tree, p_tree->data, key);
}


#endif

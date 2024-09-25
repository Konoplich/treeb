#ifndef delete_c
#define delete_c
#include <interface.c>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static u_int32_t min_keys = 1;

int delete_key_from_node(PROOT, u_int32_t key);

u_int32_t merge_child_node(PROOT, int cur_node_pos){
    int mergeidx = (*pp_root)->pp_child[cur_node_pos]->n; // merge는 왼쪽 기준으로 하는데 자식노드에서 합쳐질 위치 지정.
    // 바로 지우지 않고 합치려고 하는 노드에 지우려고 하는 부모노드(내부노드)의 값을 합침. 왜냐? 안 내리고 바로 지우고 자식노드만 합치면, 합치려고 하는 노드 밑에 또 자식노드가 있을 경우에는 자식 1개가 떠버리게 됨. 
    // 그래서 일단 부모노드의 값을 넣고 거기서 또 재귀로 들어가서 그 자식을 합치던가 빌리던가 해서 자식수를 해결해야 함.
    void_ref_t val_node = (*pp_root)->p_keys[cur_node_pos]; // 지우려는 부모 노드의 값을 기억
    (*pp_root)->pp_child[cur_node_pos]->p_keys[mergeidx] = (*pp_root)->p_keys[cur_node_pos]; 
    (*pp_root)->pp_child[cur_node_pos]->n++;

    // 합치려는 노드에 형제 노드 값을 가지고 옴. (원래 함수의 목적)
    for (int i=0; i < (*pp_root)->pp_child[cur_node_pos+1]->n; i++){
        (*pp_root)->pp_child[cur_node_pos]->p_keys[mergeidx+1+i] = (*pp_root)->pp_child[cur_node_pos+1]->p_keys[i];
        (*pp_root)->pp_child[cur_node_pos]->n++;
    }
    // 형제노드 자식도 들고와야 함.
    for (int i=0; i< (*pp_root)->pp_child[cur_node_pos+1]->n; i++){
        (*pp_root)->pp_child[cur_node_pos]->pp_child[mergeidx+1+i] = (*pp_root)->pp_child[cur_node_pos+1]->pp_child[i];
        (*pp_root)->pp_child[cur_node_pos]->n++;
    }

    // 부모노드(내부노드)의 키를 줬으니까 재배열 & 자식도 재배열
    for (int i= cur_node_pos; i<(*pp_root)->n; i++){
        (*pp_root)->p_keys[i] = (*pp_root)->p_keys[i+1];
        (*pp_root)->n--;
    }
    for (int i = cur_node_pos+1; i < (*pp_root)->n; i++){
        (*pp_root)->pp_child[i]= (*pp_root)->pp_child[i+1];
        (*pp_root)->n--;
    }
    return val_node.key; // 부모노드에서 내렸던 값을 지우기 위해 일단 리턴. 값을 삭제하는 함수에서 지워질 예정.

}

void_ref_t find_predecessor(PROOT){
    void_ref_t predecessor;
    if ((*pp_root)->leaf){ //현재 탐색노드가 리프이면, 찾을 수 있음.
        return (*pp_root)->p_keys[(*pp_root)->n-1]; //현재 노드에서 가장 큰 키 주면 됨.(predecessor 조건)
    }
    return find_predecessor(&(*pp_root)->pp_child[(*pp_root)->n-1]); // 탐색할 때마다 큰 쪽 자식으로 탐색해야 함.
}

void_ref_t find_successor(PROOT){
    void_ref_t successor;
    if ((*pp_root)->leaf){ //현재 탐색노드가 리프이면, 찾을 수 있음.
        return (*pp_root)->p_keys[0]; //현재 노드에서 가장 작은 키 주면 됨.(successor 조건)
    }
    return find_successor(&(*pp_root)->pp_child[0]); // 탐색할 때마다 작은 쪽 자식으로 탐색해야 함.
}

u_int32_t override_with_successor(PROOT, int pos_std_search){
    void_ref_t successor = find_successor(&(*pp_root)->pp_child[pos_std_search+1]); // successor를 재귀로 쭉 내려가서 찾는 함수 호출. 부모 노드랑 타고 내려갈 위치(오른쪽으로 가야됨.)를 인자로 줌.
    (*pp_root)->p_keys[pos_std_search] = successor; // 지우려고 하는 내부노드의 값에 찾은 successor로 대체 해줌.
    return successor.key;
}

u_int32_t override_with_predecessor(PROOT, int pos_std_search){
    void_ref_t predecessor = find_predecessor(&(*pp_root)->pp_child[pos_std_search]); // predecessor를 재귀로 쭉 내려가서 찾는 함수 호출. 부모 노드랑 타고 내려갈 위치를 인자로 줌.
    (*pp_root)->p_keys[pos_std_search] = predecessor; // 지우려고 하는 내부노드의 값에 찾은 predecessor로 대체 해줌.
    return predecessor.key;
}

void borrow_from_right(PROOT, int cur_node_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. n는 최소상태 혹은 미만일거임.
    int tenant_idx = (*pp_root)->pp_child[cur_node_pos]->n; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.
    (*pp_root)->pp_child[cur_node_pos]->p_keys[tenant_idx] = (*pp_root)->p_keys[cur_node_pos]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 그리고 형제노드의 키가 위로 올라가는 꼴.
    (*pp_root)->pp_child[cur_node_pos]->n++;

    int idx_from_sib_topar= 0;
    (*pp_root)->p_keys[cur_node_pos] = (*pp_root)->pp_child[cur_node_pos+1]->p_keys[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다.

    // // 형제노드는 키를 빌려줬으니까 위치 정리를 해야 한다.(한 칸씩 앞으로 땡기기)
    for (int i=0; i< ((*pp_root)->pp_child[cur_node_pos+1]->n)-1; i++){ 
        (*pp_root)->pp_child[cur_node_pos+1]->p_keys[i] = (*pp_root)->pp_child[cur_node_pos+1]->p_keys[i+1];
    }
    (*pp_root)->pp_child[cur_node_pos+1]->n--;

    int idx_from_sib = 0;
    // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
    if ((*pp_root)->pp_child[cur_node_pos+1]->n>0){ // 자식이 있는 경우에만
        int tenant_childidx = (*pp_root)->pp_child[cur_node_pos]->n; // tenant 노드에 빌려온 자식이 들어갈 위치
        (*pp_root)->pp_child[cur_node_pos]->pp_child[tenant_childidx] = (*pp_root)->pp_child[cur_node_pos+1]->pp_child[idx_from_sib]; // 형제 노드 자식 빌려옴.
        (*pp_root)->pp_child[cur_node_pos]->n++;

        // 자식 위치 정리 
        for (int i = 0; i<(*pp_root)->pp_child[cur_node_pos+1]->n-1; i++){
            (*pp_root)->pp_child[cur_node_pos+1]->pp_child[i] = (*pp_root)->pp_child[cur_node_pos+1]->pp_child[i+1];
        }
        (*pp_root)->pp_child[cur_node_pos+1]->n--;
    }
}
void merge_node(PROOT, int node_pos, int mer_node_pos){
    // 왼쪽 노드를 지웠을 때에는 최종 merge되는 주체가 왼쪽 노드가 되게 강제로 만듬.(편리를 위해) / node_pos가 삭제된 키를 갖고있는 노드가 될 수도 있고 안될수도 있음.
    int merge_idx = (*pp_root)->pp_child[mer_node_pos]->n; // merge의 주체 노드쪽에 merge될 키 위치를 지정한다.
    (*pp_root)->pp_child[mer_node_pos]->p_keys[merge_idx]= (*pp_root)->p_keys[mer_node_pos]; //부모노드의 키를 merge함.
    (*pp_root)->pp_child[mer_node_pos]->n++;

    for(int i=0; i<(*pp_root)->pp_child[node_pos]->n; i++){ // 지우는 노드에서 그 키를 지워서 최소 키 개수 유지가 안될 수 있음. 남은 키 들은 merge한 노드로 옮겨야 함. 남은키가 없으면 아예 for문이 안돌아감.
        (*pp_root)->pp_child[mer_node_pos]->p_keys[merge_idx+1+i] = (*pp_root)->pp_child[node_pos]->p_keys[i]; // 키 개수가 2개 -> i가 0, merge idx 오른쪽 키/ 키 개수가 늘어나면 i도 하나씩 더 늘어남. 
        (*pp_root)->pp_child[mer_node_pos]->n++;
    }

    int merge_childidx = (*pp_root)->pp_child[mer_node_pos]->n; //merge한 노드 끝부분으로 옮겨야 하니 끝부분 idx 지정.
    for (int i=0; i<(*pp_root)->pp_child[node_pos]->n; i++){ // 지우는 노드에서 키를 지우고 남은 자식이 있으니. 걔네를 merge한 노드로 옮겨야 한다.
        (*pp_root)->pp_child[mer_node_pos]->pp_child[merge_childidx+i] = (*pp_root)->pp_child[node_pos]->pp_child[i];
        (*pp_root)->pp_child[mer_node_pos]->n++;
    }

    free((*pp_root)->pp_child[node_pos]); // merge 되고 나서 반대편 노드는 필요없으니 메모리에서 날리기

    for(int i = mer_node_pos; i < (*pp_root)->n-1; i++) { //부모 노드의 키 하나는 이미 자식 노드와 병합되었으니 재정비해야 함.
        (*pp_root)->p_keys[i] = (*pp_root)->p_keys[i+1];
    }
    //(*pp_root)->n--;

    for(int i = mer_node_pos+1; i<(*pp_root)->n; i++){ // 부모 노드에는 병합 한 거 말고 그 뒤에 다른 자식도 있을 수 있으니 재배열 해야 함. merge한 노드 뒷 노드부터 대상이 됨.
        (*pp_root)->pp_child[i] = (*pp_root)->pp_child[i+1];
    }
    (*pp_root)->n--;
}

void borrow_from_left(PROOT, int cur_node_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. n는 최소상태 혹은 미만일거임.
    int tenant_idx = 0; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.

    // 빌리는 노드는 자리를 마련해야 하기 때문에 남아있는거를 한칸 씩 뒤로 미뤄야 함.(남아있는게 없어도 적용). borrowFromRight와 다르게 먼저 자리 정리를 해야 함.
    for (int i=0; i< (*pp_root)->pp_child[cur_node_pos]->n; i++){ 
        (*pp_root)->pp_child[cur_node_pos]->p_keys[i+1] = (*pp_root)->pp_child[cur_node_pos]->p_keys[i];
    }
    (*pp_root)->pp_child[cur_node_pos]->p_keys[tenant_idx] = (*pp_root)->p_keys[cur_node_pos-1]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 여기서는 왼쪽에서 빌려오니까, cur_node_pos(오른쪽 자식 가리키는 위치)에서 1을 빼야 부모의 키 위치가 됨. 그리고 형제노드의 키가 위로 올라가는 꼴.
    //(*pp_root)->pp_child[cur_node_pos]->n++;

    int idx_from_sib_topar= ((*pp_root)->pp_child[cur_node_pos-1]->n) -1; // 부모노드 키를 밑으로 내렸으니까 형제 노드 중 마지막 키를 하나 부모로 올려야 됨. 
    (*pp_root)->p_keys[cur_node_pos-1] = (*pp_root)->pp_child[cur_node_pos-1]->p_keys[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다. 여기선 왼쪽 형제노드.
    (*pp_root)->pp_child[cur_node_pos-1]->n--;

    // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
    if ((*pp_root)->pp_child[cur_node_pos-1]->n>0){ // 형제 노드 자식이 있는 경우에만
        int tenant_childidx = ((*pp_root)->pp_child[cur_node_pos-1]->n)-1; // tenant 노드에다가 형제노드의 가장 뒷 자식 위치를 줘야되기 때문에 그 인덱스 지정.
        // 자식 위치 정리. borrowFromRight와 다르게 옮기기 전 미리 세팅해야 함. 
        for (int i = (*pp_root)->pp_child[cur_node_pos]->n; i > 0; i--){ // 뒤에서부터 시작해서 앞에껄 뒤로 옮기는 느낌.
            (*pp_root)->pp_child[cur_node_pos]->pp_child[i] = (*pp_root)->pp_child[cur_node_pos]->pp_child[i-1];
        }

        (*pp_root)->pp_child[cur_node_pos]->pp_child[0] = (*pp_root)->pp_child[cur_node_pos-1]->pp_child[tenant_childidx]; // 형제 노드 자식 빌려옴. 빌려온 자식 놓는 위치는 현재 노드의 1번째 위치.
        (*pp_root)->pp_child[cur_node_pos]->n++;

        (*pp_root)->pp_child[cur_node_pos-1]->n--;
    }
}

void balance_node(PROOT, int child_pos){ // 현재 노드와 자식노드에서의 위치를 인자로 받는 함수(빌리기,병합을 진행)
    if (child_pos==0){ // 자식노드 키 위치가 맨 왼쪽일때는 오른쪽 부모,형제를 봐야 함.
        if ((*pp_root)->pp_child[child_pos+1]->n > min_keys){ // (자식노드 기준) 형제의 키개수가 최소숫자 범위 안 부서질때
            borrow_from_right(pp_root,child_pos);
        }
        else{ // 형제의 키개수가 최소숫자 범위 부서질때
            merge_node(pp_root,child_pos+1, child_pos); // 부모노드(현재노드)와 자신 위치랑 자기 형제 위치를 같이 넘겨줌.  
        }
        return;
    }

    else if (child_pos == (*pp_root)->n){ // 자식노드 키 위치가 맨 오른쪽일 때는 왼쪽 부모, 형제 봐야 함.
        if ((*pp_root)->pp_child[child_pos-1]->n > min_keys){ // 자식노드 기준, 왼쪽 형제의 키개수가 최소숫자 범위 안 부서질 때
            borrow_from_left(pp_root, child_pos);
        }
        else{ // 최소숫자 범위 부서질 때
            merge_node(pp_root, child_pos, child_pos-1); // 부모노드(현재노드)와 지우는 노드랑 병합대상 노드 위치를 같이 넘겨줌.  
        }
        return;
    }
    else{ // 맨 왼쪽,맨 오른쪽 말고 그 이외
        if ((*pp_root)->pp_child[child_pos-1]->n > min_keys){
            borrow_from_left(pp_root,child_pos); 
        }
        else if ((*pp_root)->pp_child[child_pos+1]->n > min_keys){
            borrow_from_right(pp_root, child_pos);
        }
        else{
            merge_node(pp_root, child_pos,child_pos-1); // 극단에 있는 자식 말고 그 외 지역에 위치한 노드들이 병합할 때
        }
        return;
    }
}

void delete_inner_node(PROOT, u_int32_t cur_pos){
    u_int32_t cessor = 0; // predecessor 혹은 successor가 있을 경우 || merge할 경우의 찾은 값을 담은 변수
    u_int32_t deletion_for_merge = 0;
    p("1");
    // 왼쪽 오른쪽 중 어느쪽 자식이 더 많은지 확인, prede혹은 successor를 찾아야 하기때문. 만약 같으면 무조건 왼쪽 보게 강제.
    if ((*pp_root)->pp_child[cur_pos]->n >= (*pp_root)->pp_child[cur_pos+1]->n){ 
        if((*pp_root)->pp_child[cur_pos]->n > min_keys){ // 자식 키개수가 최소범위 부시지 않으면 predecessor 찾기 가능.
            cessor = override_with_predecessor(pp_root, cur_pos);
            delete_key_from_node(&(*pp_root)->pp_child[cur_pos], cessor); // 찾은 predecessor를 위로 올려야 함. 근데 이 과정이 결국 해당 리프노드에서 값을 지우는게 효과라서 삭제하는 함수 호출.
        }
        else{
            deletion_for_merge = merge_child_node(pp_root, cur_pos);
            delete_key_from_node(&(*pp_root)->pp_child[cur_pos], deletion_for_merge);
        }
    }
    else {
        if ((*pp_root)->pp_child[cur_pos+1]->n > min_keys){
            cessor = override_with_successor(pp_root, cur_pos);
            delete_key_from_node(&(*pp_root)->pp_child[cur_pos+1], cessor); // successor 찾으면 이것도 리프노드에서 지우는 효과를 내야 함.
        }
        else{
            deletion_for_merge = merge_child_node(pp_root, cur_pos);
            delete_key_from_node(&(*pp_root)->pp_child[cur_pos], deletion_for_merge);
        }

    }

}

int delete_key_from_node(PROOT, u_int32_t key){ 
    int pos; // 지우려는 노드의 포지션 위치를 잡음.    
    int flag = false; // 지워졌는지 여부를 알기 위해 flag를 접수
    for (pos=0; pos < (*pp_root)->n; pos++){ // key이 지워져야하니 그 위치를 찾아야 함.현재 노드의 키 개수만큼 탐색
        if (key == (*pp_root)->p_keys[pos].key){ // 현재 노드의 키 배열에서 pos와 key이 같으면
            flag = true; // 찾았다는 표시
            break;
        }
        else if (key < (*pp_root)->p_keys[pos].key){ // 키 배열의 pos 위치 값이 key보다 크면 그 위치에서 멈춰라. 거기에서 아래로 더 들어가야 한다.
            break;
        }
    } // 이게 끝났다는건 그 노드에서 (추가 탐색할) pos위치가 정해졌다는 것
    p("flag %b\n", flag);
    if (flag){ // flag가 true이면 실제로 삭제하는 작업 실시
        if ((*pp_root)->leaf){  // 리프에서 삭제해야 하면
            p("is leaf\n");
            p("n %d, pos %d\n", (*pp_root)->n, pos);
            p("key[%d] %d\n",pos+1, (*pp_root)->p_keys[pos+1].key);
            p("key[%d] %d\n",pos, (*pp_root)->p_keys[pos].key);
            for (int i = pos; i<(*pp_root)->n; i++){ // 저장된 pos위치부터 키 개수만큼 탐색
                (*pp_root)->p_keys[i] = (*pp_root)->p_keys[i+1]; // 지우려는 키 위치에 그 다음 키로 덮어쓰기
            }            
            (*pp_root)->n--;
        }
        else { // 내부에서 삭제해야 하면
               p("not leaf\n");
                delete_inner_node(pp_root, pos); //내부 노드의 값을 삭제하는 함수 제작. 현재 노드와 현재노드에서의 값 위치를 인자로 넘김.
        }
        p("end\n");
        return flag;
    }
    else { // flag가 false이면(지우려는 값을 못찾은 것)
        if ((*pp_root)->leaf) { //leaf 노드이면
            return flag;
        }
        else{ // 지우려는 값을 못 찾았는데 내부 노드이면 더 내려감.
            flag = delete_key_from_node(&(*pp_root)->pp_child[pos], key); //key이랑 현재노드의 pos번째 자식 넘겨서 flag 받기
        }
    }
    if ((*pp_root)->pp_child[pos]->n < min_keys){ // (재귀가 끝나서 다시 올라온뒤)삭제처리했던 자식 노드의 키 개수가 최소숫자 범위 부셔졌을 때
        balance_node(pp_root, pos); // 빌리던, 병합하던 하는 함수 제작 (현재 노드와 자식노드의 pos위치를 인자로)
    }

    return flag;    
}


node_t* delete_rec(PROOT, u_int32_t key){ // 현재 노드랑 지우려는 값을 인자로 받음.
    if (!(*pp_root)){ // 현재 보는 노드에 아예 값이 없으면
        printf("Empty tree!!\n");
        return NULL;
    }
    int flag = delete_key_from_node(pp_root, key); // 현재 노드 내에서 값을 지우는 함수 호출. 지우는 값이랑 현재 노드를 인자로 받음. 리턴은 flag로 받음.
    if (!flag){ // flag가 0이면 실행.
        printf("%d does not exist in this tree. \n", key); // 함수 내에서 못찾으면 플래그가 0인거니까 에러 메세지 출력
        return NULL;
    }
    p("n: %d\n",(*pp_root)->n);
    if ((*pp_root)->n == 0){  // deleteVal을 하고 나서 (*pp_root)의 키개수가 0일 때 = 현재 노드에 아무것도 없어서 변화가 필요함.
        (*pp_root) = (*pp_root)->pp_child[0]; // 지금 노드를 가장 왼쪽 자식 노드로 만듬.
    }
    return (*pp_root);

}

void delete(TREE, u_int32_t key)
{
    if(!p_tree || !p_tree->data)
        return;
    p_tree->data = delete_rec(&p_tree->data, key);
}

#endif

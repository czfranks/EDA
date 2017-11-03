#include <bits/stdc++.h>

#define Left(x) (2*x)
#define Right(x) (1+2*x)

#define inf (2147483646)

using namespace std;

typedef pair<int,int> Point;
typedef vector<Point> vecPoint;

class nodeArray{ //Es un elemnto de yArray donde se guarda el punto
public:
    Point p;
    int pred,suc;

    nodeArray(){ init(Point(-1,-1), -1, -1); }
    nodeArray(Point pto, int pr=-1, int su=-1){ 
        init(pto, pr, su); }
    void print(){
        printf("%d<-(%d,%d)->%d\n", p.first, p.second,pred,suc);
    }
private:
    void init(Point pto, int pr, int su){
        p=pto, pred=pr, suc=su; }
};

class compareY{
public:
    compareY(){}
    bool operator()(nodeArray a, nodeArray b){
        Point& pa = a.p;
        Point& pb = b.p;
        if(pa.second == pb.second) return pa.first < pb.first;
        return pa.second < pb.second;
    }
};
class compareX{
public:
    compareX(){}
    bool operator()(nodeArray& a, nodeArray& b){
        Point& pa = a.p;
        Point& pb = b.p;
        if(pa.first == pb.first) return pa.second < pb.second;
        return pa.first < pb.first;
    }
};

//Armando la estructura del Layered Tree
typedef vector<nodeArray> yArray; // array de nodeArray ordenados por Y
typedef vector<yArray> XTree; //[vector o Arbol] con datos = yArray

class LT{
public:
    LT(){}
    ~LT(){}
    
    void build(vecPoint& _A){
        A=_A;
        sort(A.begin(), A.end());
        int n=A.size();
        int nlgn = n*(1+log(n)/log(2.0));
        xtree.clear();
        xtree.assign(nlgn,yArray());
        build(A,1,0,n-1);
        makePointers(1,0,n-1);
    }   

    int query(Point _p1, Point _p2){
        p1=_p1, p2=_p2;
        int x1 = p1.first, y1 = p1.second; //(x1,y1)
        int x2 = p2.first, y2 = p2.second; //(x2,y2)
        if(x1>x2) swap(x1,x2);
        if(y1>y2) swap(y1,y2);
        //Busqueda del rango en X
        int i,j;
        xquery(x1,x2,i,j);
        //Busqueda del rango en Y
        Point py1(-inf,y1);
        Point py2(+inf,y2);
        yArray& C = xtree[1];// accediendo a el array de la raiz;
        yArray::iterator low, up;
        low = lower_bound(C.begin(), C.end(), nodeArray(py1), compareY());
        up  = upper_bound(C.begin(), C.end(), nodeArray(py2), compareY());
        int ylow = low- C.begin();
        int yup  = up - C.begin();
        int n=A.size();
        int ans = yquery(1,0,n-1, i,j, ylow,yup);
        cout << "#nptos: " << ans << endl;
        return ans;
    }

    void print(int node, int a, int b){
        if(a==b){ printf("node: %d:\n (%d,%d)\n",
                  node, xtree[node][0].p.first, xtree[node][0].p.second); }
        else{
            int m = (a+b)/2;
            print(Left(node), a, m);
            print(Right(node), m+1, b);

            yArray& C = xtree[node];
            int nc = C.size();
            printf("node: %d:\n ", node);
            for(int i=0;i<nc;++i){
                printf("%d<-(%d,%d)->%d\n",C[i].pred,C[i].p.first, C[i].p.second,C[i].suc);
            }
        }
    }

private:

    void build(vecPoint& A, int node, int a, int b){
        if(a==b) {  //hojas
           xtree[node]=yArray({nodeArray(A[a],-1,-1)});
            //xtree[node].push_back(nodeArray(A[a]));
        }
        else{ 
            int m = (a+b)/2;
            build(A,Left(node), a, m);
            build(A,Right(node), m+1, b);

            yArray& L = xtree[Left(node)];
            yArray& R = xtree[Right(node)];
            yArray& C = xtree[node];
            int nl = L.size();
            int nr = R.size();
            xtree[node].resize(nl+nr);

            merge(L.begin(), L.end(), R.begin(), R.end(), C.begin(), compareY());   
        }
    }

    void xquery(int x1, int x2,int &i, int &j){ //nos dara el rango solucion [i,j] respecto a x
        Point px1(x1,-inf);
        Point px2(x2,+inf);
        vecPoint::iterator low, up;
        low = lower_bound(A.begin(), A.end(), px1);
        up  = upper_bound(A.begin(), A.end(), px2);
        i = low- A.begin();
        j = up - A.begin()-1;
        //cout << "ij : "<<i << " "<< j << endl;
    }

    int yquery(int node, int a, int b,/*range x:*/ int i, int j, /*rango y:*/int low, int up){
        //cout << "visit node: " << node << endl;
        if(a>j || b<i) {return 0; cout << "";} //retorna nada
        else if(a>=i && b<=j){// hay un subarbol respuesta

            //retornamos el rango [low,up] de este subarbol
            yArray& C = xtree[node];
            cout << "node: "<< node << " ->";
            int ans=0;
            for(int k=low;k<=up;++k){
                Point& p = C[k].p;
                if(p1.first<=p.first && p.first<=p2.first && p1.second<=p.second && p.second<=p2.second){
                    cout <<" (" <<C[k].p.first << ","<<C[k].p.second << ")";    
                    ++ans;
                }
            }cout << endl;
            return ans;
        }
        else{
            int m=(a+b)/2;
            //low = pred(low)
            //up  = pred(up)
            int low1 = xtree[node][low].pred;
            int up1 = xtree[node][up].pred;
            int nl = yquery(Left(node),a,m, i,j, low1,up1);
            //low = suc(low)
            //up  = suc(up)
            int low2 = xtree[node][low].suc;
            int up2 = xtree[node][up].suc;
            int nr = yquery(Right(node),m+1,b, i,j, low2,up2);

            //concatenar las respuestas de lkos arrays en una sola
            //codigo de concatenar return concat(arrayleft,arrayRight)
            return nl+nr;
        }
    }

    void makePointers(int node, int a, int b){
        if(a==b); // hojas
        else{
            int m=(a+b)/2;
            makePointers(Left(node), a, m); ///TEST -> poner al final
            makePointers(Right(node), m+1, b); /// ""

            yArray& L = xtree[Left(node)];
            yArray& R = xtree[Right(node)];
            yArray& C = xtree[node];
            int nl = L.size();
            int nr = R.size();

            int i=-1,j=-1;
            for(int k=0;k<C.size();++k){
                nodeArray& curr = C[k];
                //if(node==1) cout << "i="<<i<<", pto("<<curr.p.first<<","<<curr.p.second<<")"; 
                //----PREDECESOR
                if(curr.p == L[(i+1==nl)? i:i+1].p) //es igual a left??
                    curr.pred=++i;
                else if(i==-1) curr.pred=i+1; //Test -> else {if else}
                else curr.pred=i;
                //if(node==1) cout <<" "<< i<<"<-\n";

                //if(node==1) cout << "j="<<j<<", pto("<<curr.p.first<<","<<curr.p.second<<")"; 
                //----SUCESOR
                if(curr.p == R[(j+1==nr)? j:j+1].p) //es igual a right??
                    curr.suc=++j;
                else if(j==-1) curr.suc=j+1; //Test -> else {if else}
                else curr.suc=j+1;
                //if(node==1) cout <<" ->"<< j <<", "<< curr.suc<<"\n";

            
            }
        }
    }
    
private:
    XTree xtree;
    vecPoint A;
    Point p1, p2;
};



int main()
{
    LT layered;



    //vecPoint A = {Point(1,1),Point(2,3),Point(3,1),Point(3,4),Point(4,2),Point(5,3),Point(6,2),Point(7,4)};
    srand(time(NULL));
    int n = (int)(1e6);
    vecPoint A(n);
    map<int,map<int,int>> rep;
    for(int i=0;i<n;++i){
        int x = ((long long)rand()*100ll)%100000000;
        int y = ((long long)rand()*100ll)%100000000;
        if(!rep[x][y]){
            rep[x][y]=1;
            A[i] = Point(x,y);
        }
        else --i;
    }
    cout << "ya genero " << endl;

    //for(int i=0;i<A.size();++i) cout <<"Ai=" <<A[i].first << ","<<A[i].second << endl;
    layered.build(A);
    cout << "construyo" << endl;
    //layered.print(1,0,A.size()-1);
    int x1=456,y1=457, x2=600, y2=602;
    if(x1>x2) swap(x1,x2);
    if(y1>y2) swap(y1,y2);
    int ansLay = layered.query(Point(x1,y1),Point(x2,y2));
    int ansBrute=0;
    for(int i=0;i<n;++i){
        if(x1<=A[i].first && A[i].first<=x2 && y1<=A[i].second && A[i].second<=y2) ++ansBrute;
    }

    cout << ansLay << " vs " << ansBrute << endl; 

    return 0;
}

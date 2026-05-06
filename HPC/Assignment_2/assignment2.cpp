#include<iostream>
#include<vector>
#include<omp.h>
#include<bits/stdc++.h>
using namespace std;

void bubble(vector<int>a,bool p){
    double s=omp_get_wtime();

    for(int k=0;k<a.size();k++){
        int st=k%2;

        if(p){
            #pragma omp parallel for
            for(int i=st;i<a.size()-1;i+=2)
                if(a[i]>a[i+1]) swap(a[i],a[i+1]);
        }
        else{
            for(int i=st;i<a.size()-1;i+=2)
                if(a[i]>a[i+1]) swap(a[i],a[i+1]);
        }
    }

    cout<<(p?"Parallel ":"Sequential ")<<"Bubble Sort Time: "
        <<omp_get_wtime()-s<<endl;
}

void merge(vector<int>&a,int l,int m,int h){
    vector<int>t;
    int i=l,j=m+1;

    while(i<=m&&j<=h)
        t.push_back(a[i]<a[j]?a[i++]:a[j++]);

    while(i<=m) t.push_back(a[i++]);
    while(j<=h) t.push_back(a[j++]);

    for(int k=0;k<t.size();k++) a[l+k]=t[k];
}

void msort(vector<int>&a,int l,int h,bool p){
    if(l>=h) return;

    int m=(l+h)/2;

    if(p){
        #pragma omp parallel sections
        {
            #pragma omp section
            msort(a,l,m,p);

            #pragma omp section
            msort(a,m+1,h,p);
        }
    }
    else{
        msort(a,l,m,p);
        msort(a,m+1,h,p);
    }

    merge(a,l,m,h);
}

void runMerge(vector<int>a,bool p){
    double s=omp_get_wtime();

    msort(a,0,a.size()-1,p);

    cout<<(p?"Parallel ":"Sequential ")
        <<"Merge Sort Time: "
        <<omp_get_wtime()-s<<endl;
}

int main(){
    int n;
    cout<<"Enter size: ";
    cin>>n;

    vector<int>a(n);

    cout<<"Enter elements:\n";
    for(int&i:a) cin>>i;

    bubble(a,0);
    bubble(a,1);
    runMerge(a,0);
    runMerge(a,1);
}





####COMMANDS###
g++ -fopenmp assignment2.cpp -o assignment2
./assignment2

    

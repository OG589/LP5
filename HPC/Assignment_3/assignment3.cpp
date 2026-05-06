#include<iostream>
#include<vector>
#include<omp.h>
#include<bits/stdc++.h>
using namespace std;

void solve(vector<int>a){
    int mn=INT_MAX,mx=INT_MIN,s=0,pmn=INT_MAX,pmx=INT_MIN,ps=0;
    float avg=0,pavg=0;
    double st,en;

    st=omp_get_wtime();
    for(int i:a){
        mn=min(mn,i);
        mx=max(mx,i);
        s+=i;
        avg+=i;
    }
    en=omp_get_wtime();

    cout<<"Minimum Element: "<<mn<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Maximum Element: "<<mx<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Summation: "<<s<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Average: "<<avg/a.size()<<"\nTime Taken: "<<en-st<<endl;

    st=omp_get_wtime();

    #pragma omp parallel for reduction(min:pmn) reduction(max:pmx) reduction(+:ps,pavg)
    for(int i=0;i<a.size();i++){
        pmn=min(pmn,a[i]);
        pmx=max(pmx,a[i]);
        ps+=a[i];
        pavg+=a[i];
    }

    en=omp_get_wtime();

    cout<<"\nMinimum Element(Parallel Reduction): "<<pmn<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Maximum Element(Parallel Reduction): "<<pmx<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Summation(Parallel Reduction): "<<ps<<"\nTime Taken: "<<en-st<<endl;
    cout<<"Average(Parallel Reduction): "<<pavg/a.size()<<"\nTime Taken: "<<en-st<<endl;
}

int main(){
    int n;
    cout<<"Enter number of elements: ";
    cin>>n;

    vector<int>a(n);

    cout<<"Enter elements:\n";
    for(int&i:a) cin>>i;

    solve(a);
}



###COMMANDS###
 g++ -fopenmp assignment3.cpp -o assignment3
./assignment3




Same for max, sum, avg.

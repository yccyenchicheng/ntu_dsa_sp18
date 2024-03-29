#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <utility> // pair
#include <map>
#include <bitset> // count 1
#include <unordered_set> // record single pig
#include <climits>


using namespace std;
typedef long long int lli;

#define epsilon 10e-7
#define PIG 1
#define NOPIG 0
#define DEBUG 1

struct point {
    int x, y;
    point() {}
    point(int coor_x, int coor_y) {
        x = coor_x;
        y = coor_y;
    }
};

struct coef {
    double a;
    double b;
    coef() {}
    coef(double coef_a, double coef_b) {
        a = coef_a;
        b = coef_b;
    }


    //bool operator==(const coef& l, const coef& r) {
    //    return (l.a == r.a) && (l.b == r.b);
    //}
    void print() const {
        printf("y = %f x^2 + %f x\n", a, b);
    }
};

bool operator< (coef l, coef r) {
    return make_pair(l.a, l.b) < make_pair(r.a, r.b);
}

//int ith_pig(int num_pigs, int i) {
int ith_pig(int i) {
    // return the integer representing the ith pig
    // e.g: num_pigs = 5, i = 2
    // then ith_pig is:
    //      4 3 2 1 0
    //      0 0 1 0 0
    return (1 << i);
}

//int is_pig(int num_pigs, int pig_set, int i) {
int is_pig(int pig_set, int i) {
    return ( (pig_set >> i) & 1 ) == PIG;
    //return ((pig_set >> (num_pigs - i - 1)) & 1) == PIG;
}

void show_pigs(int num_pigs, int pig_set) {
    for (int i = num_pigs - 1; i >= 0; --i) {
        if (is_pig(pig_set, i))
           printf("%d ", PIG);
        else
           printf("%d ", NOPIG);
    }
    printf("\n");
}

int compute_single(int pig_set_single) {
    std::bitset<sizeof(int) * CHAR_BIT> b(pig_set_single);
    return b.count();
}

coef compute_coef(point& p0, point& p1) {
    // compute the coefficient of: y = a x^2 + b x
    // a < 0, x, y >= 0
    // y0 = a x0^2 + b x0
    // y1 = a x1^2 + b x1
    // b = (y1 - a x1^2) / x1
    //
    // return coef(0, 0) if invalid
    int x0, y0, x1, y1;
    x0 = p0.x;
    y0 = p0.y;
    x1 = p1.x;
    y1 = p1.y;

    if (x0 == x1) {
        // should record two parabola for both points
        return coef(0, 0);
    }

    double a = (x1 * y0 - x0 * y1) / (double) ( x0 * x1 * (x0 - x1) );
    if (a >= 0) {
        // should record two parabola for both points
        return coef(1, 0);
    }

    double b = (y1 - a * x1 * x1) / (double) x1;
    
    return coef(a, b);
}

bool is_on_parabola(coef& cf, point& p) {
    int x = p.x;
    //return (abs(x * (cf.a * x + cf.b) - p.y) <= epsilon);
    bool res = (abs(x * (cf.a * x + cf.b) - p.y) <= epsilon); // compare 2 double

    #ifdef DEBUG
    if (res)
        printf("[%d, %d] is on     y = %fx^2 + %fx \n", p.x, p.y, cf.a, cf.b);
    else
        printf("[%d, %d] is not on y = %fx^2 + %fx \n", p.x, p.y, cf.a, cf.b);
    #endif
    
    return res; 
}

//TODO iterate all pigs, compuate all coef, record all points on that parabola
//vector<coef> compute_all_parabola(vector<point>& pigs) {
void compute_all_parabola(vector<point>& pigs, map<coef, int>& map_pig, unordered_set<int>& set_non_single_pig, int& pig_set_single) {
    // if we found multiple pigs on a parabola
    // we substract it from 1111...1111 to form the pig_set_single
    int num_pigs = pigs.size();
    map<coef, int>::iterator map_it;
    unordered_set<int>::iterator set_it;

    for (int i = 0; i < num_pigs; ++i) {
        for (int j = 0; j < i; ++j) {
            #ifdef DEBUG
            printf("current point: %d: [%d, %d], %d: [%d, %d]\n", i, pigs[i].x, pigs[i].y, j, pigs[j].x, pigs[j].y);
            #endif    
        
            point pig_i = pigs[i], pig_j = pigs[j];

            coef cf = compute_coef(pig_i, pig_j);
            map_it = map_pig.find(cf);

            if (map_it != map_pig.end()) {
                int pig_set = map_it->second;
                #ifdef DEBUG
                printf("found curve: y = %f x^2 + %f x, pigs are: ", map_it->first.a, map_it->first.b);
                show_pigs(num_pigs, pig_set);
                #endif
            }  else { 
                int pig_set_i = (1 << (num_pigs - i - 1)); 
                int pig_set_j = (1 << (num_pigs - j - 1));

                if (cf.a < 0) {
                    // first we should add pigs[i], pigs[j] to the pig_set, because they must
                    // be on that parabola
                    // and we use int to record which pig is on the parabola
                    int pig_set = 0 + pig_set_i + pig_set_j; 

                    // we maintain a set to record non-single pig
                    // and update the int pig_set_single in the same time
                    if ( (set_it = set_non_single_pig.find(pig_set_i)) == set_non_single_pig.end() ) {
                        set_non_single_pig.insert(pig_set_i);
                        pig_set_single -= pig_set_i;
                    }

                    if ( (set_it = set_non_single_pig.find(pig_set_j)) == set_non_single_pig.end() ) {
                        set_non_single_pig.insert(pig_set_j);
                        pig_set_single -= pig_set_j;
                    }

                    // TODO add not single pigs into unordered_set

                    for (int k = 0; k < num_pigs; ++k) { // iterate all pigs to record 
                        if (k != i && k != j) {
                            if (is_on_parabola(cf, pigs[k])) {
                                int pig_set_k = (1 << (num_pigs - k - 1));

                                if ( (set_it = set_non_single_pig.find(pig_set_k)) == set_non_single_pig.end() ) {
                                    set_non_single_pig.insert(pig_set_k);
                                    pig_set_single -= pig_set_k;
                                }

                                pig_set += pig_set_k;
                            }
                        }
                    
                    }

                    map_pig[cf] = pig_set;
                    #ifdef DEBUG
                    printf("y = %f x^2 + %f x has pigs: ", cf.a, cf.b);
                    show_pigs(num_pigs, pig_set);
                    #endif
                } else {
                //else if (cf.a == 0) {
                    // case: x0 == x1, suppose we always assign a = -1
                    // y = ax^2 + bx
                    // b = (y + x^2) / x
                    
                    /* record single pig in map_pig is inefficient. so we comment out here. */
                    //int xi = pig_i.x, yi = pig_i.y; 
                    //int xj = pig_j.x, yj = pig_j.y;
            
                    //coef coef_i = coef(-1, (yi + xi * xi)/ (double)xi );
                    //coef coef_j = coef(-1, (yj + xj * xj)/ (double)xj );

                    //pig_set_single = pig_set_single + pig_set_i + pig_set_j;

                    //map_pig[coef_i] = pig_set_i;
                    //map_pig[coef_j] = pig_set_j;


                    #ifdef DEBUG
                    printf("current point: [%d, %d], [%d, %d] can't form a parabola with a < 0.\n", pigs[i].x, pigs[i].y, pigs[j].x, pigs[j].y);
                    #endif
                }
             }
        }
    }
}

// TODO
int dp(map<int, int>& ans, vector<point>& pigs, int pig_set, map<coef, int>& map_pig) {
    if (pig_set == 0) {
        return 0;
    }

    if ( (pig_set & (pig_set - 1) ) == 0 ) { // power of 2 => then we have only 1 bird. return 1
        return 1;
    }
    
    // TODO use another map record the results of dp
    // map<int, int> map_dp;  {first: second} = {pig_set: num_shots}

    // dp[S] = min (dp[S / A]) + 1, for all A where A = pigs on a certain parabola
    // S = pig_set = 11111...1111, initially
    // iterate through the parabola
    int min_total = 1000000007;
    int total = 0;
    int tmp;
    //for (auto& cf_pigset : map_pig) {
    map<coef, int>::iterator map_pig_end = map_pig.end();
    map<int, int>::iterator ans_it;
    for (map<coef, int>::iterator it = map_pig.begin(); it != map_pig_end; ++it) {
        // pig_left is S XOR with the pigs on the parabola
        // TODO implement S / A
        // should also check if pig_set is on the map_pig
        // ex: S = 11111, A = 00100 => ~A = 11011
        //
        //     11111 &
        //     11011
        //     -----
        //     11011
        
        int pig_left = pig_set & (~ it->second);
        #ifdef DEBUG
        printf("parabola is: ");
        it->first.print();

        printf("pigs before: ");
        show_pigs(pigs.size(), pig_set);

        printf("pigs remained: ");
        show_pigs(pigs.size(), pig_left);
        #endif
    
        map<coef, int> map_pig_copy = map_pig;
        map_pig_copy.erase(it->first);

        if ( (ans_it = ans.find(pig_left)) != ans.end() ) {
            tmp = ans_it->second;
        } else {
            // proceed only if pig_set has changed
            if (pig_left != pig_set) {
        
            // find min (dp[S / A])
            tmp = dp(ans, pigs, pig_left, map_pig_copy); 
            }
        }

        if (tmp < min_total) {
            min_total = tmp;
        }
    }
    
    ans[pig_set] = min_total + 1;
    return min_total + 1;
}

int dp2(map<int, int>& ans, vector<point>& pigs, int pig_set, map<coef, int> map_pig) {
    if (pig_set == 0) {
        return 0;
    }

    if ( (pig_set & (pig_set - 1) ) == 0 ) { // power of 2 => then we have only 1 bird. return 1
        return 1;
    }
    
    int min_total = 10e7;
    int total = 0;
    int tmp;

    map<int, int>::iterator ans_it;
    for (map<coef, int>::iterator it = map_pig.begin(); it != map_pig.end(); ++it) {
        // pig_left is S XOR with the pigs on the parabola
        // TODO implement S / A
        // should also check if pig_set is on the map_pig
        // ex: S = 11111, A = 00100 => ~A = 11011
        //
        //     11111 &
        //     11011
        //     -----
        //     11011
        
        int pig_left = pig_set & (~ it->second);
        show_pigs(pigs.size(), pig_left);
        #ifdef DEBUG
        printf("parabola is: ");
        it->first.print();

        printf("pigs before: ");
        show_pigs(pigs.size(), pig_set);

        printf("pigs remained: ");
        show_pigs(pigs.size(), pig_left);
        #endif
        
        // proceed only if pig_set has changed
        // find min (dp[S / A])
        if ( (ans_it =  ans.find(pig_left)) != ans.end() ) {
            tmp = ans_it->second;
        } else {
            map_pig.erase(it);
            tmp = dp(ans, pigs, pig_left, map_pig);
        }

        if (tmp < min_total) {
            min_total = tmp;
        }
    }
    
    ans[pig_set] = min_total + 1;
    return min_total + 1;
}

template <typename T>
void print(vector<T> v) {
    for (int i = 0; i < v.size(); ++i)
        cout << v[i] << " ";
    cout << "\n";
}

void print(vector<point> v) {
    for (int i = 0; i < v.size(); ++i ) {
        printf("[%d, %d] ", v[i].x, v[i].y);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    
    int T; // number of test cases
    fscanf(stdin, "%d", &T);

    for (int i = 0; i < T; ++i) {
        int n; // number of pigs 
        fscanf(stdin, "%d", &n);

        map<coef, int> map_pig;
        unordered_set<int> set_non_single_pig;
        
        vector<point> pigs(n); // vector of the location of the pigs
        int x, y;
        //point loc_pig;
        for (int j = 0; j < n; ++j) {
            fscanf(stdin, "%d %d", &x, &y);
            pigs[j] = point(x, y);
        }
        

        // set pig_set = 111111111, n 1's
        // so we want 000000111111, n 1's
        int pig_set = (1 << n) - 1;
        int pig_set_single = pig_set; // initialize as the same as pig_set
        #ifdef DEBUG
        print(pigs); 
        printf("showing initial %d pigs: \n", n);
        show_pigs(n, pig_set);
        #endif

        /*
        compute_all_parabola(pigs, map_pig, set_non_single_pig, pig_set_single);
        
        // compute the # of single pigs
        int num_single = compute_single(pig_set_single);

        // dp to solve the total shots needed for non-single pigs
        int pig_set_not_single = pig_set & (~ pig_set_single);
        #ifdef DEBUG
        printf("num_single: %d\n", num_single);
        printf("num_nonsingle: %d\n", compute_single(pig_set_not_single));
        printf("show single pigs: ");
        show_pigs(n, pig_set_single);
        printf("show non_single pigs: ");
        show_pigs(n, pig_set_not_single);
        #endif

        map<int, int> ans;
        int total_shots = dp(ans, pigs, pig_set_not_single, map_pig) + num_single;
        //int total_shots = dp2(ans, pigs, pig_set_not_single, map_pig) + num_single;
        printf("%d\n", total_shots);
        */
    }
    return 0;
}

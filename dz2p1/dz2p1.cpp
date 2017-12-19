#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <fstream>

using namespace std;

struct process {
	string name;
	int Tcomplete, MaxWT, waitingTime, executionTime;
	process(string i,int tc, int mwt) : Tcomplete(tc), MaxWT(mwt),name(i) {
		waitingTime = 0;
		executionTime = 0;
	}
};



class tree {


	struct node {
		process *p[3];
		node* child[4];
		node* parent;
		int n;
		node(process *tmp) {
			p[1] = tmp;
			p[0] = p[2] = nullptr;
			child[0] = child[1] = child[2] = child[3] = nullptr;
			n = 1;
		}
	};

	node* root=nullptr;

public:

	void insert(process*);
	void remove(node*, int);
	process* findX(int);
	node* vratiCvorX(int);
	friend node* leftBrother(node*);
	friend node* rightBrother(node*);
	friend ostream& operator<<(ostream&, tree&);
	void deleteTree();
	friend void Simulation(tree& T);
	friend bool isLeaf(node* tmp);
	friend int main();
};

void tree::insert(process *t) {

	int tc, mwt;
	string ime;
	
	process *tmp;
	tmp = t;

	if (root == nullptr) {     // PRAZNO STABLO

		root = new node(tmp);  
		root->parent = nullptr;
	}
	else {
		node *tren,*left,*right;
		stack<node*> s;
		tren = root;
		
		while (tren != nullptr) {
			s.push(tren);
			int i,poz;
			
			if (tren->n == 1) {
				if (tren->p[1]->waitingTime > tmp->waitingTime) {
					poz = 1;
				}
				else {
					poz = 2;
				}

			}
			else if (tren->n == 2) {
				
				if (tren->p[0] == nullptr) {
					poz = 3;
					for (i = 1; i <= 2; i++) {
						if (tren->p[i]->waitingTime > tmp->waitingTime) {
							poz = i;
							break;
						}
					}
				}
				else {
					poz = 2;
					for (i = 0; i <= 1; i++) {
						if (tren->p[i]->waitingTime > tmp->waitingTime) {
							poz = i;
							break;
						}
					}
				}
			}
			else {
				poz = 3;
				for (i = 0; i <= 2; i++) {
					if (tren->p[i]->waitingTime > tmp->waitingTime) {
						poz = i;
						break;
					}
				}
			}

			tren = tren->child[poz];

		} // end of while stack

		left = right = nullptr;

		while (!s.empty()) {
			tren = s.top();
			s.pop();
			if (tren->n == 1) {
				if (tmp->waitingTime >= tren->p[1]->waitingTime) {
					tren->p[2] = tmp;
					tren->child[2] = left;
					tren->child[3] = right;
				}
				else {
					tren->p[0] = tmp;
					tren->child[0] = left;
					tren->child[1] = right;
				}
				tren->n++;
				break;
			} // UBACIVANJE U CVOR SA JEDNIM ELEMENTOM
			else if (tren->n == 2) {
				if (tren->p[0] == nullptr) {
					if (tmp->waitingTime >= tren->p[1]->waitingTime) {
						tren->p[0] = tren->p[1];
						tren->child[0] = tren->child[1];

						if (tmp->waitingTime >= tren->p[2]->waitingTime) {
							tren->p[1] = tren->p[2];
							tren->child[1] = tren->child[2];
							tren->p[2] = tmp;
							tren->child[2] = left;
							tren->child[3] = right;
						}
						else {
							tren->p[1] = tmp;
							tren->child[1] = left;
							tren->child[2] = right;
						}
					}
					else {
						tren->p[0] = tmp;
						tren->child[0] = left;
						tren->child[1] = right;
					}

				}
				else {
					if (tmp->waitingTime < tren->p[1]->waitingTime) {
						tren->p[2] = tren->p[1];
						tren->child[3] = tren->child[2];

						if (tmp->waitingTime < tren->p[0]->waitingTime) {
							tren->p[1] = tren->p[0];
							tren->child[2] = tren->child[1];
							tren->p[0] = tmp;
							tren->child[0] = left;
							tren->child[1] = right;
						}
						else {
							tren->p[1] = tmp;
							tren->child[1] = left;
							tren->child[2] = right;
						}
					}
					else {
						tren->p[2] = tmp;
						tren->child[2] = left;
						tren->child[3] = right;
					}
				}

				tren->n++;
				break;
			}  // UBACIVANJE U CVOR SA 2 ELEMENTA
			else {

				node *l,*d;
				process *mid;
				mid = tren->p[1];

				l = new node(tren->p[0]);
				l->child[1] = tren->child[0];
				l->child[2] = tren->child[1];
				d = new node(tren->p[2]);
				d->child[1] = tren->child[2];
				d->child[2] = tren->child[3];

				if (tmp->waitingTime < mid->waitingTime) {
					if (tmp->waitingTime >= tren->p[0]->waitingTime) {
						l->p[2] = tmp;
						l->child[2] = left;
						l->child[3] = right;
					}
					else {
						l->p[0] = tmp;
						l->child[0] = left;
						l->child[1] = right;
					}
					l->n++;
				}
				else {
					if (tmp->waitingTime >= tren->p[2]->waitingTime) {
						d->p[2] = tmp;
						d->child[2] = left;
						d->child[3] = right;
					}
					else {
						d->p[0] = tmp;
						d->child[0] = left;
						d->child[1] = right;
					}
					d->n++;
					
				}

				for (int i = 0; i <= 3; i++) {
					if (l->child[i] != nullptr) l->child[i]->parent = l;
					if (d->child[i] != nullptr)d->child[i]->parent = d;
				}

				left = l;
				right = d;

				if (s.empty()) {
					node* nov;
					nov = new node(mid);
					nov->child[1] = left;
					nov->child[2] = right;
					left->parent = nov;
					right->parent = nov;
					root = nov;
					root->parent = nullptr;
				}
				else {
					tmp = mid;
					left->parent = s.top();
					right->parent = s.top();
				}

			} // UBACIVANJE U CVOR SA 3
		}

	} // end of else

}


process* tree::findX(int x) {
	
	int i,poz;
	node* tren = root;

	while (tren != nullptr) {
	
		int i, poz;

		if (tren->n == 1) {

			if (tren->p[1]->waitingTime == x) return tren->p[1];

			if (tren->p[1]->waitingTime > x) {
				poz = 1;
			}
			else {
				poz = 2;
			}

		}
		else if (tren->n == 2) {

			if (tren->p[0] == nullptr) {
				poz = 3;

				if (tren->p[1]->waitingTime == x) return tren->p[1];
				if (tren->p[2]->waitingTime == x) return tren->p[2];

				for (i = 1; i <= 2; i++) {
					if (tren->p[i]->waitingTime > x) {
						poz = i;
						break;
					}
				}
			}
			else {
				if (tren->p[1]->waitingTime == x) return tren->p[1];
				if (tren->p[0]->waitingTime == x) return tren->p[0];
				poz = 2;
				for (i = 0; i <= 1; i++) {
					if (tren->p[i]->waitingTime > x) {
						poz = i;
						break;
					}
				}
			}
		}
		else {
			poz = 3;
			for (i = 0; i <= 2; i++) {
				if (tren->p[i]->waitingTime == x) return tren->p[i];
				if (tren->p[i]->waitingTime > x) {
					poz = i;
					break;
				}
			}
		}

		tren = tren->child[poz];

	} // end of while stack

	return nullptr;
}

tree::node* tree::vratiCvorX(int x) {
	int i, poz;
	node* tren = root;

	while (tren != nullptr) {

		int i, poz;

		if (tren->n == 1) {

			if (tren->p[1]->waitingTime == x) return tren;

			if (tren->p[1]->waitingTime > x) {
				poz = 1;
			}
			else {
				poz = 2;
			}

		}
		else if (tren->n == 2) {

			if (tren->p[0] == nullptr) {
				poz = 3;

				if (tren->p[1]->waitingTime == x) return tren;
				if (tren->p[2]->waitingTime == x) return tren;

				for (i = 1; i <= 2; i++) {
					if (tren->p[i]->waitingTime > x) {
						poz = i;
						break;
					}
				}
			}
			else {
				if (tren->p[1]->waitingTime == x) return tren;
				if (tren->p[0]->waitingTime == x) return tren;
				poz = 2;
				for (i = 0; i <= 1; i++) {
					if (tren->p[i]->waitingTime > x) {
						poz = i;
						break;
					}
				}
			}
		}
		else {
			poz = 3;
			for (i = 0; i <= 2; i++) {
				if (tren->p[i]->waitingTime == x) return tren;
				if (tren->p[i]->waitingTime > x) {
					poz = i;
					break;
				}
			}
		}

		tren = tren->child[poz];

	} // end of while stack

	return nullptr;
}

void tree::remove(node* tmp,int pos) {

	
	node* tren = tmp;
	node* succ = tren;
	int succpos = pos;
	process* q;
	node *lbrat, *dbrat,*del, *roditelj,*child = nullptr;
	int i, pozicija;
	bool nadjen;


	if (!isLeaf(tren)) {
		succ = tren->child[pos + 1];
		while (!isLeaf(succ)) {
			if (succ->p[0] != nullptr) {
				succ = succ->child[0];
			}
			else {
				succ = succ->child[1];
			}
		}

		if (succ->p[0] != nullptr) succpos = 0; else succpos = 1;
		


		q = tren->p[pos];
		tren->p[pos] = succ->p[succpos];
		succ->p[succpos] = q;

		tren = succ;
		pos = succpos;

	}

	
	//delete tren->p[pos];
	tren->p[pos] = nullptr;
	tren->n--;


		if (tren->n > 0) {

			if (pos == 1) {
				if (tren->p[0] != nullptr)
				{
					tren->p[pos] = tren->p[0];
					tren->p[0] = nullptr;
				}
				else {
					tren->p[pos] = tren->p[2]; 
					tren->p[2] = nullptr;
				}
			}
		
		} // UKOLIKO OSTAJE DOVOLJNO KLJUCEVA
		else {

			while (tren->parent != nullptr && tren->n==0) {

				lbrat = leftBrother(tren);
				dbrat = rightBrother(tren);
				roditelj = tren->parent;


				if (lbrat != nullptr && lbrat->n > 1) {


					lbrat->n--;

					for (i = 0; i <= 3; i++) {
						if (roditelj->child[i] == lbrat) {
							pozicija = i;
							break;
						}
					}

					tren->p[1] = roditelj->p[pozicija];
					tren->child[2] = child;
					tren->child[1] = nullptr;

					if (lbrat->p[2] != nullptr) {

						roditelj->p[pozicija] = lbrat->p[2];
						lbrat->p[2] = nullptr;

						tren->child[1] = lbrat->child[3]; // ovo
						if (lbrat->child[3]) lbrat->child[3]->parent = tren;
						lbrat->child[3] = nullptr;

					}
					else {

						roditelj->p[pozicija] = lbrat->p[1];
						lbrat->p[1] = lbrat->p[0];
						lbrat->p[0] = nullptr;

						tren->child[1] = lbrat->child[2]; //ovo
						if (lbrat->child[2]) lbrat->child[2]->parent = tren;
						lbrat->child[2] = nullptr;

						lbrat->child[2] = lbrat->child[1];
						lbrat->child[1] = lbrat->child[0];
						lbrat->child[0] = nullptr;
					}
					tren->n++;

				} // KRAJ PRESIPANJA IZ LEVOG BRATA
				else if (dbrat != nullptr && dbrat->n > 1) {

					dbrat->n--;

					for (i = 0; i <= 3; i++) {
						if (roditelj->child[i] == dbrat) {
							pozicija = i - 1;
							break;
						}
					}

					tren->p[1] = roditelj->p[pozicija];
					tren->child[1] = child;
					tren->child[2] = nullptr;

					if (dbrat->p[0] != nullptr) {

						roditelj->p[pozicija] = dbrat->p[0];
						dbrat->p[0] = nullptr;

						tren->child[2] = dbrat->child[0];
						if (dbrat->child[0]) dbrat->child[0]->parent = tren;
						dbrat->child[0] = nullptr;
			
					}
					else {

						roditelj->p[pozicija] = dbrat->p[1];
						dbrat->p[1] = dbrat->p[2];
						dbrat->p[2] = nullptr;

						tren->child[2] = dbrat->child[1];
						if (dbrat->child[1]) dbrat->child[1]->parent = tren;
						dbrat->child[1] = nullptr;

						dbrat->child[1] = dbrat->child[2];
						dbrat->child[2] = dbrat->child[3];
						dbrat->child[3] = nullptr;

					}

					tren->n++;

				} // KRAJ PRESIPANJA IZ DESNOG BRATA
				else {

					if (lbrat != nullptr) {

						//cout << "USAO " << lbrat->p[1]->name << lbrat->n << endl;
						
						roditelj->n--;
						lbrat->n++;

						for (i = 0; i <= 3; i++) {
							if (roditelj->child[i] == lbrat) pozicija = i;
						}

						//roditelj->child[pozicija + 1] = nullptr;

						lbrat->p[0] = lbrat->p[1];
						lbrat->child[0] = lbrat->child[1];
						lbrat->child[1] = lbrat->child[2];
						lbrat->p[1] = roditelj->p[pozicija];
						lbrat->child[2] = child;


						if (child != nullptr) child->parent = lbrat;

						if (pozicija == 1) {
							
							if (roditelj->p[0] != nullptr) {
								roditelj->p[1] = roditelj->p[0];
								roditelj->child[2] = roditelj->child[1];
								roditelj->child[1] = roditelj->child[0];
								roditelj->child[0] = nullptr;
								roditelj->p[0] = nullptr;

							}
							else if (roditelj->p[2] != nullptr) {
					
								roditelj->p[1] = roditelj->p[2];
								roditelj->child[2] = roditelj->child[3];
								roditelj->child[3] = nullptr;
								roditelj->p[2] = nullptr;
							}
							else {
								roditelj->p[1] = nullptr;
							}
						}
						else if (pozicija == 0) {
							roditelj->p[0] = nullptr;
							roditelj->child[1] = roditelj->child[0];
							roditelj->child[0] = nullptr;
						}
						else {
							roditelj->p[2] = nullptr;
							roditelj->child[3] = nullptr;
						}

						child = lbrat;
						if (roditelj->parent == nullptr && roditelj->n == 0) {
							root = lbrat;
							root->parent = nullptr;
							
						}

					}
					else if (dbrat != nullptr) {
						
						roditelj->n--;
						dbrat->n++;
						


						for (i = 0; i <= 3; i++) {
							if (roditelj->child[i] == dbrat) pozicija = i-1;
						}

						//roditelj->child[pozicija] = nullptr;

						dbrat->p[2] = dbrat->p[1];
						dbrat->child[3] = dbrat->child[2];
						dbrat->child[2] = dbrat->child[1];
						dbrat->p[1] = roditelj->p[pozicija];
						dbrat->child[1] = child;


						if (child != nullptr) child->parent = dbrat;

						if (pozicija == 1) {
							if (roditelj->p[0] != nullptr) {
								roditelj->p[1] = roditelj->p[0];
								roditelj->child[1] = roditelj->child[0];
								roditelj->child[0] = nullptr;
								roditelj->p[0] = nullptr;

							}
							else if (roditelj->p[2] != nullptr) {
								roditelj->p[1] = roditelj->p[2];
								roditelj->child[1] = roditelj->child[2];
								roditelj->child[2] = roditelj->child[3];
								roditelj->child[3] = nullptr;
								roditelj->p[2] = nullptr;
							}
							else {
								roditelj->p[1] = nullptr;
							}
						}
						else if (pozicija == 0) {
							roditelj->p[0] = nullptr;
							roditelj->child[0] = nullptr;
						}
						else {
							roditelj->p[2] = nullptr;
							roditelj->child[2] = roditelj->child[3];
							roditelj->child[3] = nullptr;
						}

						child = dbrat;
						
						if (roditelj->parent == nullptr && roditelj->n == 0) {
							root = dbrat;
							root->parent = nullptr;
							
						}
					}
					
					
					del = tren;
					tren = tren->parent;
					//delete del;

				} // SPAJANJE SE RADI


				//tren = tren->parent;
			
			} // KRAJ WHILE

		} // UKOLIKO NE OSTAJE DOVOLJNO KLJUCEVA

		nadjen = false;
		for (i = 0; i <= 2; i++) if (root->p[i] != nullptr) nadjen = true;

		if (!nadjen) root = nullptr;


}

bool isLeaf(tree::node* tmp) {
	
	for (int i = 0; i <= 3; i++) if (tmp->child[i] != nullptr) return false;

	return true;
}

tree::node* leftBrother(tree::node* tmp) {
	
	tree::node* roditelj = tmp->parent;

	if (roditelj == nullptr) return nullptr;

	for (int i = 0; i <= 3; i++) {
		if (roditelj->child[i] == tmp && i!=0) return roditelj->child[i - 1];
	}

	return nullptr;
}

tree::node* rightBrother(tree::node* tmp) {
	tree::node* roditelj = tmp->parent;
	
	if (roditelj == nullptr) return nullptr;

	for (int i = 0; i <= 3; i++) {
		if (roditelj->child[i] == tmp && i!=3) return roditelj->child[i+1];
	}

	return nullptr;
}

ostream& operator<<(ostream& os,tree& t) {

	tree::node* tren;
	queue<tree::node*> q;

	if (t.root == nullptr) {
		return os << ">------Prazno stablo------<" << endl;
	}
	q.push(t.root);

	os << ">----------------- ISPIS KAO 2-3-4 STABLO --------------------<" << endl;

	while (!q.empty()) {
		tren = q.front();
		q.pop();

		for (int i = 0; i <= 2; i++) {
			if (tren->p[i] == nullptr) {
				os << "null | ";
			}
			else {
				os << tren->p[i]->name << " | ";
			}
			
		}

		
		if (tren->parent != nullptr) os << tren->parent->p[1]->name; else os << "null";
		os << endl;

	/*	os << "DECA : ";

		for (int i = 0; i <= 3; i++) {
			if (tren->child[i]) {
				os << tren->child[i]->p[1]->name<< " ";
			}
			else {
				os << "null ";
			}
		}

		os << endl;*/
		for (int i = 0; i <= 3; i++) {
			if (tren->child[i] != nullptr) q.push(tren->child[i]);
		}

	}
	
	os << ">-------------- ISPIS KAO CRVENO-CRNO STABLO --------------<" << endl;

	q.push(t.root);

	while (!q.empty()) {
		tren = q.front();
		q.pop();

		for (int i = 0; i <= 2; i++) {
			if (tren->p[i] != nullptr) {
			
				os << tren->p[i]->name << " ";
				if (i == 0 || i == 2) {
					os << "Red" << endl;
				}
				else {
					os << "Black" << endl;
				}
			}
		}
		for (int i = 0; i <= 3; i++) {
			if (tren->child[i] != nullptr) q.push(tren->child[i]);
		}
	}

	return os << ">----------------------------------------------------<" << endl;
}

void tree::deleteTree() {

	int i;
	node* tmp;
	stack<node*> s;

	if (root == nullptr) return;

	s.push(root);

	while (!s.empty()) {
		tmp = s.top();
		s.pop();
		for (i = 0; i <= 3; i++) {
			if (tmp->child[i] != nullptr) s.push(tmp->child[i]);
		}
		for (i = 0; i <= 2; i++) {
			if (tmp->p[i] != nullptr) delete tmp->p[i];
		}
		delete tmp;
	}

	root = nullptr;
}

void Simulation(tree &T) {


	process *trenutni;
	tree::node* tmp,*tren;
	int inc,mesto,brojac=1,novinc;
	queue<process*> q;
	stack<tree::node*> s;
	fstream datoteka;

	datoteka.open("simulacija.txt", fstream::out);

	datoteka << "Start of execeution" << endl;

	cout << "Unesite inkrement vremena: ";
	cin >> inc;

	tmp = T.root;

	while (T.root != nullptr) {

		tmp = T.root;
		while (tmp->child[0] != nullptr || (tmp->p[0]==nullptr && tmp->child[1] != nullptr)) {
			if (tmp->child[0] != nullptr) {
				tmp = tmp->child[0];
			}
			else {
				tmp = tmp->child[1];
			}
		}

		if (tmp->p[0] != nullptr) {
			mesto = 0;
		}
		else {
			mesto = 1;
		}

		trenutni = tmp->p[mesto];

		cout << "Time(" << brojac <<  ") Current process: " << trenutni->name << " | Execution time: " << trenutni->executionTime << " | Waiting time: " << trenutni->waitingTime << endl;
		datoteka << ">---------------------------------------------------------------------------<" << endl;
		datoteka << "Time(" << brojac << ") Current process: " << trenutni->name << " | Execution time: " << trenutni->executionTime << " | Waiting time: " << trenutni->waitingTime << endl;

		brojac++;

		novinc = inc;

		/*cout << T;
		cout << endl;*/

		T.remove(tmp, mesto);

	/*	cout << T;
		cout << endl;*/
		
		trenutni->waitingTime += inc;

		if (trenutni->waitingTime >= trenutni->MaxWT) {
			//trenutni->waitingTime -= trenutni->MaxWT;
			trenutni->waitingTime %= trenutni->MaxWT;

		}
		


		if (trenutni->executionTime + inc < trenutni->Tcomplete) {
			q.push(trenutni);
			trenutni->executionTime += inc;
		
		}
		else {
			novinc = trenutni->Tcomplete - trenutni->executionTime;
			
		}

		


		if (T.root!=nullptr) s.push(T.root);

		while (!s.empty()) {
			tren = s.top();
			s.pop();

			for (int i = 0; i <= 2; i++) {
				if (tren->p[i] != nullptr) {
					tren->p[i]->waitingTime += novinc;

					if (tren->p[i]->waitingTime >= tren->p[i]->MaxWT) {
				
						tren->p[i]->waitingTime %= tren->p[i]->MaxWT;
						
						
					}


					q.push(tren->p[i]);
					//cout << "IZBACIO JE: " << tren->p[i]->name << tren->p[i]->waitingTime << endl;
					//T.remove(tren, i);
/*
					cout << T;
					cout << endl;*/

				}
			}

			for (int i = 0; i <= 3; i++) {
				if (tren->child[i] != nullptr) {
					s.push(tren->child[i]);
				}
			}

		}

		T.root = nullptr;


		while (!q.empty()) {
			trenutni = q.front();
			//cout << trenutni->name << " ";

			T.insert(trenutni);

			/*cout << "UBACIO JE: " << trenutni->waitingTime << endl;
			cout << T;
			cout << endl;*/

			q.pop();
		}

		cout << endl;

		
		

	}
	
	datoteka << ">---------------------------------------------------------------------------<" << endl;
	datoteka << "End of execution" << endl;
	datoteka.close();
}


int main() {

	tree T;
	process *nov;
	string naziv;
	int tc,mwt,komanda,x,poz;
	string name, imedat;
	fstream datoteka;
	
	
	while (1) {
		cout << ">------------------ MENI ------------------<" << endl;
		cout << "1) Formiraj stablo na osnovu datoteke" << endl;
		cout << "2) Obrisi stablo" << endl;
		cout << "3) Dodaj proces u stablo" << endl;
		cout << "4) Brisanje procesa iz stabla" << endl;
		cout << "5) Pretrazi stablo" << endl;
		cout << "6) Ispis stabla" << endl;
		cout << "7) Simulacija rada rasporedjivaca procesa" << endl;
		cout << "8) Izlaz" << endl;
		cout << ">------------------------------------------<" << endl;
		cout << "Unesite opciju: ";
		cin >> komanda;

		switch (komanda) {
		case 1:

			T.deleteTree();

			cout << "Ucitaj naziv datoteke:" << endl;
			cin >> imedat;

			datoteka.open(imedat, fstream::in);

			while (!datoteka.eof()) {
				datoteka >> name >> tc >> mwt;
				nov = new process(name, tc, mwt);
				T.insert(nov);
			}

			datoteka.close();

			break;
		case 2:
			T.deleteTree();
			break;
		case 3:
			cout << "Unesite naziv procesa: ";
			cin >> naziv;
			cout << "Unesite vreme potrebno za zavrsetak: ";
			cin >> tc;
			cout << "Unesite maksimalno vreme cekanja: ";
			cin >> mwt;
			nov = new process(naziv, tc, mwt);
			T.insert(nov);
			break;
		case 4:

			
			cin >> x;
			tree::node* tmp;
			tmp = T.vratiCvorX(x);
			
			if (tmp != nullptr) {

				for (int i = 0; i <= 2; i++)
					if (tmp->p[i] != nullptr && tmp->p[i]->waitingTime == x) {
						poz = i;
						break;
					}

				T.remove(tmp, poz);
			}
			else {
				cout << "Proces nije pronadjen u stablu." << endl;
			}
			break;
		case 5:
		
			cout << "Unesite vrednost WaitingTime-a procesa koji zelite da nadjete: ";
			cin >> x;

			if (T.findX(x) != nullptr) {
				cout << "Proces je nadjen." << endl;
				
			}
			else {
				cout << "Proces nije nadjen." << endl;
			}

			break;
		case 6:
			cout << T;
			break;
		case 7:
			Simulation(T);
			break;
		case 8:
			exit(0);

		}

	}


	return 0;
}
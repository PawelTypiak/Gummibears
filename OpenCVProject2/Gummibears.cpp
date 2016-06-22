#include "opencv2/opencv.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;
using namespace cv;

int main(int, char)
{
	float skala = 0.25;
	Mat kolo, prostokat, naroznik;
	Mat kolo_dst, prostokat_dst, naroznik_dst;
	
	//creating markers' patterns
	//source: http://docs.opencv.org/2.4/modules/core/doc/drawing_functions.html

	Mat img = Mat::zeros(200, 200, CV_8UC1);
	
	img.copyTo(kolo);
	img.copyTo(prostokat);
	img.copyTo(naroznik);

	vector<Point> pkt_prostokat_prev, pkt_naroznik_prev;
	vector<vector<Point>> pkt_prostokat, pkt_naroznik;

	//creating rectangle
	pkt_prostokat_prev.push_back(Point2f(30, 75));
	pkt_prostokat_prev.push_back(Point2f(img.cols - 30, 75));
	pkt_prostokat_prev.push_back(Point2f(img.cols - 30, img.rows - 80));
	pkt_prostokat_prev.push_back(Point2f(30, img.rows - 80));
	
	pkt_prostokat.push_back(pkt_prostokat_prev);

	//creating L-marker
	pkt_naroznik_prev.push_back(Point2f(30, 30));
	pkt_naroznik_prev.push_back(Point2f(img.cols - 30, 30));
	pkt_naroznik_prev.push_back(Point2f(img.cols - 30, 75));
	pkt_naroznik_prev.push_back(Point2f(75, 75));
	pkt_naroznik_prev.push_back(Point2f(75, img.rows - 30));
	pkt_naroznik_prev.push_back(Point2f(30, img.rows - 30));

	pkt_naroznik.push_back(pkt_naroznik_prev);

	//creating circle
	circle(kolo, Point2f(kolo.cols / 2, kolo.rows / 2), 70, 255, -1, 8);
	fillPoly(prostokat, pkt_prostokat, 255);
	fillPoly(naroznik, pkt_naroznik, 255);

	//finding edges of markers
	Canny(kolo, kolo_dst, 20, 200, 3);
	Canny(prostokat, prostokat_dst, 20, 200, 3);
	Canny(naroznik, naroznik_dst, 20, 200, 3);
	
	vector<vector<Point> > contours_kolo, contours_prostokat, contours_naroznik;

	//finding contours of markers
	findContours(kolo_dst, contours_kolo, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<Moments> momenty_kolo(contours_kolo.size());
	for (int i = 0; i < contours_kolo.size(); i++)
	{
		momenty_kolo[i] = moments(contours_kolo[i], false);		//moments - circle
	}

	vector<Point2f>srodki_kolo(contours_kolo.size());
	for (int i = 0; i < contours_kolo.size(); i++)
	{
		srodki_kolo[i] = Point2f(momenty_kolo[i].m10 / momenty_kolo[i].m00, momenty_kolo[i].m01 / momenty_kolo[i].m00);		//center of gravity - circle
	}

	findContours(prostokat_dst, contours_prostokat, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<Moments> momenty_prostokat(contours_prostokat.size());
	for (int i = 0; i < contours_prostokat.size(); i++)
	{
		momenty_prostokat[i] = moments(contours_prostokat[i], false);	//moments - rectangle
	}

	vector<Point2f>srodki_prostokat(contours_prostokat.size());
	for (int i = 0; i < contours_prostokat.size(); i++)
	{
		srodki_prostokat[i] = Point2f(momenty_prostokat[i].m10 / momenty_prostokat[i].m00, momenty_prostokat[i].m01 / momenty_prostokat[i].m00);		//center of gravity - rectangle
	}

	findContours(naroznik_dst, contours_naroznik, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<Moments> momenty_naroznik(contours_naroznik.size());
	for (int i = 0; i < contours_naroznik.size(); i++)
	{
		momenty_naroznik[i] = moments(contours_naroznik[i], false);	//moments - Lmarker
	}

	vector<Point2f>srodki_naroznik(contours_naroznik.size());
	for (int i = 0; i < contours_naroznik.size(); i++)
	{
		srodki_naroznik[i] = Point2f(momenty_naroznik[i].m10 / momenty_naroznik[i].m00, momenty_naroznik[i].m01 / momenty_naroznik[i].m00);		//center of gravity - Lmarker
	}

	//opening txt file with the names of pictures, source: http://cpp0x.pl/kursy/Kurs-C++/Obsluga-plikow/305
	vector<string> nazwy;
	fstream plik;
	plik.open("images_names/images_names.txt", ios::in);
	if (plik.good())
	{
		string napis;
		
		while (!plik.eof())
		{
			getline(plik, napis);
			nazwy.push_back(napis);
		}
		plik.close();
	}
	else cout << "Error! Nie udalo otworzyc sie pliku!" << endl;

	//load pictures into vector of mats
	vector <Mat> obrazy;
	for (int i = 0; i < nazwy.size()-1; i++){
		nazwy[i] = "images/" + nazwy[i];

		obrazy.push_back(imread(nazwy[i]));
	}
	
	//opening txt with results
	ofstream plik_wyniki; 
	plik_wyniki.open("results/results.txt");
	
	cout << "wczytalo_zdjecia" << endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int nr_sceny,nr_sceny_przed,nr_sceny_po;
	bool ostatni=0;
	bool nowy=1;
	
	vector<int> zelki_biale_ujecia, zelki_zolte_ujecia, zelki_pomaranczowe_ujecia,zelki_zielone_ujecia, zelki_jasnoczerwone_ujecia,zelki_ciemnoczerwone_ujecia;
	vector<int> zelki_biale_usegregowane, zelki_zolte_usegregowane, zelki_pomaranczowe_usegregowane, zelki_zielone_usegregowane, zelki_jasnoczerwone_usegregowane, zelki_ciemnoczerwone_usegregowane;

	//processing of every picture
	for (int ujecie = 0; ujecie < obrazy.size(); ujecie++)
	{
		ostatni = 0;
		nowy = 0;

		//checking if current picture is first or last in the scene
		if(ujecie > 0 && ujecie<obrazy.size()-1){
			string scena(nazwy[ujecie].begin() + 14, nazwy[ujecie].begin() + 17);
			string scena_przed(nazwy[ujecie-1].begin() + 14, nazwy[ujecie-1].begin() + 17);
			string scena_po(nazwy[ujecie+1].begin() + 14, nazwy[ujecie+1].begin() + 17);
			
			nr_sceny = atoi(scena.c_str());
			nr_sceny_przed = atoi(scena_przed.c_str());
			nr_sceny_po = atoi(scena_po.c_str());

			if (nr_sceny != (nr_sceny_po)) ostatni=1;
			if (nr_sceny != (nr_sceny_przed)) nowy = 1;
		}
		else if(ujecie==0) nowy=1;
		else if (ujecie == (obrazy.size() - 1)) ostatni = 1;

		Mat src, src_maly, src_bin, dst;
		int b, g, r, s;
		typedef Vec<uchar, 3> Pixel;
		obrazy[ujecie].copyTo(src);

		resize(src, src_maly, Size(), skala, skala);	//resizing pictures for better performance
		src_maly.copyTo(src_bin);
		
		//changing dark pixels into white
		for (int i = 0; i < src_maly.rows; i++){
			for (int j = 0; j < src_maly.cols; j++){
				b = src_maly.at<Pixel>(i, j).val[0];
				g = src_maly.at<Pixel>(i, j).val[1];
				r = src_maly.at<Pixel>(i, j).val[2];

				if (b < 50 && g < 50 && r < 50) {
					b = src_bin.at<Pixel>(i, j).val[0] = 255;
					g = src_bin.at<Pixel>(i, j).val[1] = 255;
					r = src_bin.at<Pixel>(i, j).val[2] = 255;
				}
				else{
					b = src_bin.at<Pixel>(i, j).val[0] = 0;
					g = src_bin.at<Pixel>(i, j).val[1] = 0;
					r = src_bin.at<Pixel>(i, j).val[2] = 0;
				}
			}
		}

		//converting into grayscale
		cvtColor(src_bin, dst, CV_BGR2GRAY);

		//finding contours on current picture
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		vector<Moments> momenty(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			momenty[i] = moments(contours[i], false);
		}

		vector<Point2f>srodki(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			srodki[i] = Point2f(momenty[i].m10 / momenty[i].m00, momenty[i].m01 / momenty[i].m00);
		}

		vector<vector<Point> > contours_duze;
		vector<Point2f>srodki_duze;
		Point2f center;
		float radius;
		int area;

		//finding contours that fulfill the criterias of markers (proper area and diameter of circumcircle)
		for (int i = 0; i < contours.size(); i++)
		{
			minEnclosingCircle(contours[i], center, radius);
			area = contourArea(contours[i]);
			if (radius>10 && radius < 60 && area>250 && area < 5000) {
				contours_duze.push_back(contours[i]);
				srodki_duze.push_back(srodki[i]);
			}
		}

		double porownanie_kolo;
		double porownanie_prostokat;
		double porownanie_naroznik;
		vector<int> odleglosci;

		//finding the distance of every found object from the middle of current picture
		for (int i = 0; i < contours_duze.size(); i++)
		{
			odleglosci.push_back(abs(pointPolygonTest(contours_duze[i], Point2f(src_maly.cols / 2, src_maly.rows / 2), true)));
		}

		//extracting markers - finding 4 objects that are the closest to the middle of picture
		int mini = src_maly.cols;
		int miejsce = 0;
		vector<vector<Point> > contours_znacznik;
		vector<Point2f>srodki_znacznik;
		vector<int> odleglosci_posortowane;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < odleglosci.size(); j++)
			{
				if (mini >= odleglosci[j]) {
					miejsce = j;
					mini = odleglosci[j];
				}
			}
			contours_znacznik.push_back(contours_duze[miejsce]);
			srodki_znacznik.push_back(srodki_duze[miejsce]);
			mini = src_maly.cols;
			odleglosci[miejsce] = src_maly.cols;
		}

		//finding what type of markers we have found
		int rodzaj = 0;
		double najmniejszy = 1;
		for (int i = 0; i < contours_znacznik.size(); i++)
		{
			porownanie_kolo = matchShapes(contours_znacznik[i], contours_kolo[0], 1, 0.0);
			porownanie_prostokat = matchShapes(contours_znacznik[i], contours_prostokat[0], 1, 0.0);
			porownanie_naroznik = matchShapes(contours_znacznik[i], contours_naroznik[0], 1, 0.0);

			if (min(najmniejszy, porownanie_kolo) == porownanie_kolo)
			{
				najmniejszy = porownanie_kolo;
				rodzaj = 1;
			}

			if (min(najmniejszy, porownanie_prostokat) == porownanie_prostokat)
			{
				najmniejszy = porownanie_prostokat;
				rodzaj = 2;
			}

			if (min(najmniejszy, porownanie_naroznik) == porownanie_naroznik)
			{
				najmniejszy = porownanie_naroznik;
				rodzaj = 3;
			}
	
		//creating mask of markers
		Mat znaczniki, znaczniki_gray, tmp;
		znaczniki = Mat::zeros(src_maly.rows, src_maly.cols, CV_8UC3);
		for (int i = 0; i < contours_znacznik.size(); i++){
			drawContours(znaczniki, contours_znacznik, i, Scalar(255, 255, 255), CV_FILLED, 8);
		}
		
		cvtColor(znaczniki, znaczniki_gray, CV_BGR2GRAY);

		//sorting corners fromleft to right (min x - max x)
		vector<Point2f> rogi(4);
		vector<Point2f> rogi_uporzadkowane;
		vector<Point2f> rogi_wycinanie;
		float rogi_x_mini = znaczniki_gray.cols;
		float rogi_x_maxi = 0;
		int pozycja = 0;

		//findning if circle or Lmarker
		if (rodzaj == 1 || rodzaj == 3)
		{
			//saving centers of gravity of markers into vector
			for (int i = 0; i < 4; i++)
			{
				rogi[i] = srodki_znacznik[i];
			}

			//sorting
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (rogi_x_mini >= rogi[j].x) {

						rogi_x_mini = rogi[j].x;
						pozycja = j;
					}
				}
				rogi_uporzadkowane.push_back(rogi[pozycja]);
				rogi_x_mini = znaczniki_gray.cols;
				rogi[pozycja].x = znaczniki_gray.cols;
			}
		}

		//finding if rectangle
		else
		{
			Vec4f linia0, linia1, linia2, linia3;
			int lewo0, lewo2, lewo3, lewo1;
			int prawo0, prawo2, prawo3, prawo1;
			float x_przeciecie0, x_przeciecie1, x_przeciecie2, x_przeciecie3;
			float y_przeciecie0, y_przeciecie1, y_przeciecie2, y_przeciecie3;
			float a0, a1, a2, a3;
			float b1, b0, b2, b3;
			Mat ero;
			vector<vector<Point> > cont_prostokat_ero;

			erode(znaczniki_gray, ero, getStructuringElement(MORPH_ELLIPSE, Size(7, 7))); //erode rectangles to make it look like a line

			//fitting lines in rectangles
			findContours(ero, cont_prostokat_ero, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			fitLine(Mat(cont_prostokat_ero[0]), linia0, 2, 0, 0.01, 0.01);
			fitLine(Mat(cont_prostokat_ero[1]), linia1, 2, 0, 0.01, 0.01);
			fitLine(Mat(cont_prostokat_ero[2]), linia2, 2, 0, 0.01, 0.01);
			fitLine(Mat(cont_prostokat_ero[3]), linia3, 2, 0, 0.01, 0.01);

			//calculating the equation of a line y=ax+b
			a0 = linia0[1] / linia0[0];
			a1 = linia1[1] / linia1[0];
			a2 = linia2[1] / linia2[0];
			a3 = linia3[1] / linia3[0];

			//finding if a is big, if yes x is almost constant (vertical), that makes problems, so we need to incline it a little bit
			if (abs(a0) > 1000) {
				linia0[1] = 1000;
				linia0[0] = 1;
				a0 = linia0[1] / linia0[0];
				tmp = 10;
			}
			if (abs(a1) > 1000) {
				linia1[1] = 1000;
				linia1[0] = 1;
				a1 = linia1[1] / linia1[0];
				tmp = 1;
			}
			if (abs(a2) > 1000) {
				linia2[1] = 1000;
				linia2[0] = 1;
				a2 = linia2[1] / linia2[0];
				tmp = 2;
			}
			if (abs(a3) > 1000) {
				linia3[1] = 1000;
				linia3[0] = 1;
				a3 = linia3[1] / linia3[0];
				tmp = 3;
			}

			b0 = (-linia0[2] * linia0[1] / linia0[0]) + linia0[3];
			b1 = (-linia1[2] * linia1[1] / linia1[0]) + linia1[3];
			b2 = (-linia2[2] * linia2[1] / linia2[0]) + linia2[3];
			b3 = (-linia3[2] * linia3[1] / linia3[0]) + linia3[3];

			//finding where lines intersect
			x_przeciecie0 = (b1 - b0) / (a0 - a1);
			y_przeciecie0 = a1*x_przeciecie0 + b1;

			x_przeciecie1 = (b2 - b0) / (a0 - a2);
			y_przeciecie1 = a2*x_przeciecie1 + b2;

			x_przeciecie2 = (b1 - b3) / (a3 - a1);
			y_przeciecie2 = a1*x_przeciecie2 + b1;

			x_przeciecie3 = (b2 - b3) / (a3 - a2);
			y_przeciecie3 = a2*x_przeciecie3 + b2;

			//saving corners only if there are on the picture
			if (x_przeciecie0 > 0 && x_przeciecie0<znaczniki_gray.cols && y_przeciecie0>0 && y_przeciecie0 < znaczniki_gray.rows)
			{
				rogi[0] = (Point2f(x_przeciecie0, y_przeciecie0));
			}
			if (x_przeciecie1 > 0 && x_przeciecie1<znaczniki_gray.cols && y_przeciecie1>0 && y_przeciecie1 < znaczniki_gray.rows)
			{
				rogi[1] = (Point2f(x_przeciecie1, y_przeciecie1));
			}
			if (x_przeciecie2 > 0 && x_przeciecie2<znaczniki_gray.cols && y_przeciecie2>0 && y_przeciecie2 < znaczniki_gray.rows)
			{
				rogi[2] = (Point2f(x_przeciecie2, y_przeciecie2));
			}
			if (x_przeciecie3 > 0 && x_przeciecie3<znaczniki_gray.cols && y_przeciecie3>0 && y_przeciecie3 < znaczniki_gray.rows)
			{
				rogi[3] = (Point2f(x_przeciecie3, y_przeciecie3));
			}

			//if corners are not on the picture, that means that there was wrong order of finding lines when finding intersections, so we have to do it in different order
			if (rogi[0] == Point2f(0, 0) || rogi[1] == Point2f(0, 0) || rogi[2] == Point2f(0, 0) || rogi[3] == Point2f(0, 0))
			{
				x_przeciecie0 = (b1 - b0) / (a0 - a1);
				y_przeciecie0 = a1*x_przeciecie0 + b1;
				rogi[0] = Point2f(x_przeciecie0, y_przeciecie0);

				x_przeciecie1 = (b2 - b1) / (a1 - a2);
				y_przeciecie1 = a2*x_przeciecie1 + b2;
				rogi[1] = Point2f(x_przeciecie1, y_przeciecie1);

				x_przeciecie2 = (b3 - b2) / (a2 - a3);
				y_przeciecie2 = a3*x_przeciecie2 + b3;
				rogi[2] = Point2f(x_przeciecie2, y_przeciecie2);

				x_przeciecie3 = (b0 - b3) / (a3 - a0);
				y_przeciecie3 = a0*x_przeciecie3 + b0;
				rogi[3] = Point2f(x_przeciecie3, y_przeciecie3);
			}

			//sorting corners from min x
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (rogi_x_mini >= rogi[j].x) {
						
						rogi_x_mini = rogi[j].x;
						pozycja = j;
					}
				}
				rogi_uporzadkowane.push_back(rogi[pozycja]);
				rogi_x_mini = znaczniki_gray.cols;
				rogi[pozycja].x = znaczniki_gray.cols;
			}
		}

		//finding the right order of connecting corners - sheet of paper have to be horizontal
		float roznica_01y, roznica_02x, roznica_12y, roznica_13y, roznica_02y, roznica_03y, roznica_21x, roznica_23x, roznica_31x;
		roznica_01y = abs(rogi_uporzadkowane[0].y - rogi_uporzadkowane[1].y);
		roznica_02x = abs(rogi_uporzadkowane[0].x - rogi_uporzadkowane[2].x);
		roznica_12y = abs(rogi_uporzadkowane[1].y - rogi_uporzadkowane[2].y);
		roznica_13y = abs(rogi_uporzadkowane[1].y - rogi_uporzadkowane[3].y);
		roznica_02y = abs(rogi_uporzadkowane[0].y - rogi_uporzadkowane[2].y);
		roznica_03y = abs(rogi_uporzadkowane[0].y - rogi_uporzadkowane[3].y);
		roznica_21x = abs(rogi_uporzadkowane[2].x - rogi_uporzadkowane[1].x);
		roznica_23x = abs(rogi_uporzadkowane[2].x - rogi_uporzadkowane[3].x);
		roznica_31x = abs(rogi_uporzadkowane[3].x - rogi_uporzadkowane[1].x);


		if (roznica_01y > roznica_02x) {
			if (roznica_02y < roznica_03y){
				if (roznica_21x < roznica_23x){
					//cout << "0,2,1,3";
					rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
				}
				else{
					//cout << "0,2,3,1";
					rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
				}
			}
			else{
				if (roznica_31x < roznica_23x){
					//cout << "0,3,1,2";
					rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
				}
				else{
					//cout << "0,3,2,1";
					rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
					rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
				}
			}
		}
		else{
			if (roznica_12y < roznica_13y){
				//cout << "0,1,2,3";
				rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
			}
			else{
				//cout << "0,1,3,2";
				rogi_wycinanie.push_back(rogi_uporzadkowane[0]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[1]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[3]);
				rogi_wycinanie.push_back(rogi_uporzadkowane[2]);
			}
		}

		//cutting 
#if 0   //high resolution - more detailed image
		for (int i = 0; i < 4; i++)
		{
			rogi_wycinanie[i] = (1 / skala)*rogi_wycinanie[i];
		}

		Mat trans(3, 3, CV_32F);
		Mat po_wycieciu;
		vector<Point2f> wyjscie;
		wyjscie.push_back(Point2f(0, src.rows));
		wyjscie.push_back(Point2f(0, 0));
		wyjscie.push_back(Point2f(src.cols, 0));
		wyjscie.push_back(Point2f(src.cols, src.rows));

		float skala2 = 1.5;
		trans = getPerspectiveTransform(rogi_wycinanie, wyjscie);
		warpPerspective(src, po_wycieciu, trans, src.size());
		resize(po_wycieciu, po_wycieciu, Size(), skala*(1.5*src.rows / src.cols), skala);

		//resize(src,src,Size(),0.25,0.25,1);
#else 
		Mat trans(3, 3, CV_32F);
		Mat po_wycieciu;
		vector<Point2f> wyjscie;
		wyjscie.push_back(Point2f(0, src_maly.rows));
		wyjscie.push_back(Point2f(0, 0));
		wyjscie.push_back(Point2f(src_maly.cols, 0));
		wyjscie.push_back(Point2f(src_maly.cols, src_maly.rows));

		float skala2 = 1.5; //ratio 2/3 - a4 paper format
		trans = getPerspectiveTransform(rogi_wycinanie, wyjscie);
		warpPerspective(src_maly, po_wycieciu, trans, src_maly.size());
		resize(po_wycieciu, po_wycieciu, Size(), skala2*src_maly.rows / src_maly.cols, 1);

#endif
		//imshow("obraz", po_wycieciu);
		//waitKey(1);

		Mat kolory;
		po_wycieciu.copyTo(kolory);

		Mat biale = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);
		Mat zolte = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);
		Mat pomaranczowe = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);
		Mat zielone = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);
		Mat jasnoczerwone = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);
		Mat ciemnoczerwone = Mat::zeros(po_wycieciu.rows, po_wycieciu.cols, CV_8UC1);

		//finding gummibears in different colors and standarizing them
		for (int i = 0; i < po_wycieciu.rows; i++){
			for (int j = 0; j < po_wycieciu.cols; j++){
				b = po_wycieciu.at<Pixel>(i, j).val[0];
				g = po_wycieciu.at<Pixel>(i, j).val[1];
				r = po_wycieciu.at<Pixel>(i, j).val[2];

				//different rules for white
				if (b >= 80 && b <= 135 && g >= 120 && g <= 180 && r >= 150 && r <= 210){
					biale.at<uchar>(i, j) = 255;
				}

				//yellow
				if (b <= 60 && g >= 96 && g <= 170 && r >= 141 && r <= 225){
					kolory.at<Pixel>(i, j).val[0] = 30;
					kolory.at<Pixel>(i, j).val[1] = 120;
					kolory.at<Pixel>(i, j).val[2] = 180;
				}
				//orange
				else if (b <= 45 && g >= 50 && g <= 95 && r >= 145 && r <= 220) {
					kolory.at<Pixel>(i, j).val[0] = 30;
					kolory.at<Pixel>(i, j).val[1] = 70;
					kolory.at<Pixel>(i, j).val[2] = 180;
				}
				//green
				else if (b <= 70 && g >= 53 && g <= 130 && r >= 50 && r <= 140) {
					kolory.at<Pixel>(i, j).val[0] = 30;
					kolory.at<Pixel>(i, j).val[1] = 80;
					kolory.at<Pixel>(i, j).val[2] = 80;
				}
				//light red
				else if (b <= 45 && g <= 45 && r >= 120 && r <= 175){
					kolory.at<Pixel>(i, j).val[0] = 30;
					kolory.at<Pixel>(i, j).val[1] = 30;
					kolory.at<Pixel>(i, j).val[2] = 150;
				}
				//dark red
				else if (b <= 65 && g <= 50 && r >= 65 && r <= 170) {
					kolory.at<Pixel>(i, j).val[0] = 30;
					kolory.at<Pixel>(i, j).val[1] = 30;
					kolory.at<Pixel>(i, j).val[2] = 100;
				}
				//if different color, set black
				else {
					kolory.at<Pixel>(i, j).val[0] = 255;
					kolory.at<Pixel>(i, j).val[1] = 255;
					kolory.at<Pixel>(i, j).val[2] = 255;
				}
			}
		}

		//median filter to delete the sharp edges - colors
		medianBlur(kolory, kolory, 11);

		//deleting sharp edges - white
		dilate(biale, biale, getStructuringElement(MORPH_ELLIPSE, Size(9, 9)));
		erode(biale, biale, getStructuringElement(MORPH_ELLIPSE, Size(9, 9)));
		medianBlur(biale, biale, 15);
		//imshow("obraz0", src_kolory);
		//waitKey(0);

		//saving different colors to different variables
		for (int i = 0; i < kolory.rows; i++){
			for (int j = 0; j < kolory.cols; j++){
				b = kolory.at<Pixel>(i, j).val[0];
				g = kolory.at<Pixel>(i, j).val[1];
				r = kolory.at<Pixel>(i, j).val[2];

				//yellow
				if (b == 30 && g == 120 && r == 180){
					zolte.at < uchar>(i, j) = 255;
				}
				//orange
				else if (b == 30 && g == 70 && r == 180) {
					pomaranczowe.at < uchar>(i, j) = 255;
				}
				//green
				else if (b == 30 && g == 80 && r == 80) {
					zielone.at < uchar>(i, j) = 255;
				}
				//light red
				else if (b == 30 && g == 30 && r == 150){
					jasnoczerwone.at < uchar>(i, j) = 255;
				}
				//dark red
				else if (b == 30 && g == 30 && r == 100) {
					ciemnoczerwone.at < uchar>(i, j) = 255;
				}
			}
		}

		vector<vector<Point> > contours_biale, contours_zolte, contours_pomaranczowe, contours_zielone, contours_jasnoczerwone,contours_ciemnoczerwone;
		
		//clearing vector if new scene
		if (nowy == 1)
		{
			zelki_biale_ujecia.clear();
			zelki_zolte_ujecia.clear();
			zelki_pomaranczowe_ujecia.clear();
			zelki_jasnoczerwone_ujecia.clear();
			zelki_ciemnoczerwone_ujecia.clear();
			zelki_zielone_ujecia.clear();

			zelki_biale_usegregowane.clear();
			zelki_zolte_usegregowane.clear();
			zelki_pomaranczowe_usegregowane.clear();
			zelki_zielone_usegregowane.clear();
			zelki_jasnoczerwone_usegregowane.clear();
			zelki_ciemnoczerwone_usegregowane.clear();		
		}
			
        int zelki_biale=0, zelki_zolte=0, zelki_pomaranczowe=0, zelki_zielone=0, zelki_jasnoczerwone=0, zelki_ciemnoczerwone=0;

		//computing how many objects in different colors are on the image
		//white
		findContours(biale, contours_biale, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_biale;
		for (int i = 0; i < contours_biale.size(); i++)
		{
			momenty_biale.push_back(moments(contours_biale[i], false));
			if (contourArea(contours_biale[i])>900 && contourArea(contours_biale[i]) <= 5000)
			{
				zelki_biale++;
			}
			else if (contourArea(contours_biale[i]) > 5000 && contourArea(contours_biale[i]) <= 8000)
			{
				zelki_biale = zelki_biale + 2;
			}
			else if (contourArea(contours_biale[i]) > 8000 && contourArea(contours_biale[i]) <= 11000)
			{
				zelki_biale = zelki_biale + 3;
			}
			else if (contourArea(contours_biale[i]) > 11000 && contourArea(contours_biale[i]) <= 14000)
			{
				zelki_biale = zelki_biale + 4;
			}
			else if (contourArea(contours_biale[i]) > 14000 && contourArea(contours_biale[i]) <= 17000)
			{
				zelki_biale = zelki_biale + 5;
			}
			else if (contourArea(contours_biale[i]) > 17000 && contourArea(contours_biale[i]) <= 25000)
			{
				zelki_biale = zelki_biale + 6;
			}		
		}
		
		//yellow
		findContours(zolte, contours_zolte, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_zolte;
		for (int i = 0; i < contours_zolte.size(); i++)
		{
			momenty_zolte.push_back(moments(contours_zolte[i], false));

			if (contourArea(contours_zolte[i])>1200 && contourArea(contours_zolte[i]) <= 4200)
			{
				zelki_zolte++;
			}
			else if (contourArea(contours_zolte[i]) > 4200 && contourArea(contours_zolte[i]) <= 7000)
			{
				zelki_zolte = zelki_zolte + 2;
			}
			else if (contourArea(contours_zolte[i]) > 7000 && contourArea(contours_zolte[i]) <= 9200)
			{
				zelki_zolte = zelki_zolte + 3;
			}
			else if (contourArea(contours_zolte[i]) > 9200 && contourArea(contours_zolte[i]) <= 11500)
			{
				zelki_zolte = zelki_zolte + 4;
			}
			else if (contourArea(contours_zolte[i]) > 11500 && contourArea(contours_zolte[i]) <= 14000)
			{
				zelki_zolte = zelki_zolte + 5;
			}
		}

		//orange
		findContours(pomaranczowe, contours_pomaranczowe, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_pomaranczowe;
		for (int i = 0; i < contours_pomaranczowe.size(); i++)
		{
			momenty_pomaranczowe.push_back(moments(contours_pomaranczowe[i], false));

			if (contourArea(contours_pomaranczowe[i])>1200 && contourArea(contours_pomaranczowe[i]) <= 4200)
			{
				zelki_pomaranczowe = zelki_pomaranczowe + 1;
			}
			else if (contourArea(contours_pomaranczowe[i]) > 4200 && contourArea(contours_pomaranczowe[i]) <= 7000)
			{
				zelki_pomaranczowe = zelki_pomaranczowe + 2;
			}
			else if (contourArea(contours_pomaranczowe[i]) > 7000 && contourArea(contours_pomaranczowe[i]) <= 9200)
			{
				zelki_pomaranczowe = zelki_pomaranczowe + 3;
			}
			else if (contourArea(contours_pomaranczowe[i]) > 9200 && contourArea(contours_pomaranczowe[i]) <= 11500)
			{
				zelki_pomaranczowe = zelki_pomaranczowe + 4;
			}
			else if (contourArea(contours_pomaranczowe[i]) > 11500 && contourArea(contours_pomaranczowe[i]) <= 14000)
			{
				zelki_pomaranczowe = zelki_pomaranczowe + 5;
			}
		}

		//green
		findContours(zielone, contours_zielone, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_zielone;
		for (int i = 0; i < contours_zielone.size(); i++)
		{
			momenty_zielone.push_back(moments(contours_zielone[i], false));

			if (contourArea(contours_zielone[i])>1200 && contourArea(contours_zielone[i]) <= 4200)
			{
				zelki_zielone = zelki_zielone + 1;
			}
			else if (contourArea(contours_zielone[i]) > 4200 && contourArea(contours_zielone[i]) <= 7000)
			{
				zelki_zielone = zelki_zielone + 2;
			}
			else if (contourArea(contours_zielone[i]) > 7000 && contourArea(contours_zielone[i]) <= 9200)
			{
				zelki_zielone = zelki_zielone + 3;
			}
			else if (contourArea(contours_zielone[i]) > 9200 && contourArea(contours_zielone[i]) <= 11500)
			{
				zelki_zielone = zelki_zielone + 4;
			}
			else if (contourArea(contours_zielone[i]) > 11500 && contourArea(contours_zielone[i]) <= 14000)
			{
				zelki_zielone = zelki_zielone + 5;
			}
		}

		//light red
		findContours(jasnoczerwone, contours_jasnoczerwone, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_jasnoczerwone;
		for (int i = 0; i < contours_jasnoczerwone.size(); i++)
		{
			momenty_jasnoczerwone.push_back(moments(contours_jasnoczerwone[i], false));

			if (contourArea(contours_jasnoczerwone[i])>1200 && contourArea(contours_jasnoczerwone[i]) <= 4200)
			{
				zelki_jasnoczerwone = zelki_jasnoczerwone + 1;
			}
			else if (contourArea(contours_jasnoczerwone[i]) > 4200 && contourArea(contours_jasnoczerwone[i]) <= 7000)
			{
				zelki_jasnoczerwone = zelki_jasnoczerwone + 2;
			}
			else if (contourArea(contours_jasnoczerwone[i]) > 7000 && contourArea(contours_jasnoczerwone[i]) <= 9200)
			{
				zelki_jasnoczerwone = zelki_jasnoczerwone + 3;
			}
			else if (contourArea(contours_jasnoczerwone[i]) > 9200 && contourArea(contours_jasnoczerwone[i]) <= 11500)
			{
				zelki_jasnoczerwone = zelki_jasnoczerwone + 4;
			}
			else if (contourArea(contours_jasnoczerwone[i]) > 11500 && contourArea(contours_jasnoczerwone[i]) <= 14000)
			{
				zelki_jasnoczerwone = zelki_jasnoczerwone + 5;
			}
		}

		//dark red
		findContours(ciemnoczerwone, contours_ciemnoczerwone, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Moments> momenty_ciemnoczerwone;
		for (int i = 0; i < contours_ciemnoczerwone.size(); i++)
		{
			momenty_ciemnoczerwone.push_back(moments(contours_ciemnoczerwone[i], false));

			if (contourArea(contours_ciemnoczerwone[i])>1200 && contourArea(contours_ciemnoczerwone[i]) <= 4200)
			{
				zelki_ciemnoczerwone = zelki_ciemnoczerwone + 1;
			}
			else if (contourArea(contours_ciemnoczerwone[i]) > 4200 && contourArea(contours_ciemnoczerwone[i]) <= 7000)
			{
				zelki_ciemnoczerwone = zelki_ciemnoczerwone + 2;
			}
			else if (contourArea(contours_ciemnoczerwone[i]) > 7000 && contourArea(contours_ciemnoczerwone[i]) <= 9200)
			{
				zelki_ciemnoczerwone = zelki_ciemnoczerwone + 3;
			}
			else if (contourArea(contours_ciemnoczerwone[i]) > 9200 && contourArea(contours_ciemnoczerwone[i]) <= 11500)
			{
				zelki_ciemnoczerwone = zelki_ciemnoczerwone + 4;
			}
			else if (contourArea(contours_ciemnoczerwone[i]) > 11500 && contourArea(contours_ciemnoczerwone[i]) <= 14000)
			{
				zelki_ciemnoczerwone = zelki_ciemnoczerwone + 5;
			}
		}

		zelki_biale_ujecia.push_back(zelki_biale);
		zelki_zolte_ujecia.push_back(zelki_zolte);
		zelki_pomaranczowe_ujecia.push_back(zelki_pomaranczowe);
		zelki_zielone_ujecia.push_back(zelki_zielone);
		zelki_jasnoczerwone_ujecia.push_back(zelki_jasnoczerwone);
		zelki_ciemnoczerwone_ujecia.push_back(zelki_ciemnoczerwone);
		
		//sorting the number of objects in particular colors from min to max for every image in current scene
		
		int pozycja_biale = 0;
		int pozycja_zolte = 0;
		int pozycja_pomaranczowe = 0;
		int pozycja_zielone = 0;
		int pozycja_jasnoczerwone = 0;
		int pozycja_ciemnoczerwone = 0;

		int mediana_biale;
		int mediana_zolte;
		int mediana_pomaranczowe;
		int mediana_zielone;
		int mediana_jasnoczerwone;
		int mediana_ciemnoczerwone;
	
		int ilosc_ujec;

		int biale_min = 100, zolte_min = 100, pomaranczowe_min = 100, zielone_min = 100, jasnoczerwone_min = 100, ciemnoczerwone_min = 100;
		if (ostatni == 1) {	
			ilosc_ujec = zelki_zolte_ujecia.size();
			for (int i = 0; i < ilosc_ujec; i++){		
				for (int j = 0; j < ilosc_ujec; j++){
					
					if (biale_min >= zelki_biale_ujecia[j]){
						biale_min = zelki_biale_ujecia[j];
						pozycja_biale = j;
					}
					if (zolte_min >= zelki_zolte_ujecia[j]){
						zolte_min = zelki_zolte_ujecia[j];
						pozycja_zolte = j;
					}
					if (pomaranczowe_min >= zelki_pomaranczowe_ujecia[j]){
						pomaranczowe_min = zelki_pomaranczowe_ujecia[j];
						pozycja_pomaranczowe = j;
					}
					if (zielone_min >= zelki_zielone_ujecia[j]){
						zielone_min = zelki_zielone_ujecia[j];
						pozycja_zielone = j;
					}
					if (jasnoczerwone_min >= zelki_jasnoczerwone_ujecia[j]){
						jasnoczerwone_min = zelki_jasnoczerwone_ujecia[j];
						pozycja_jasnoczerwone = j;
					}
					if (ciemnoczerwone_min >= zelki_ciemnoczerwone_ujecia[j]){
						ciemnoczerwone_min = zelki_ciemnoczerwone_ujecia[j];
						pozycja_ciemnoczerwone = j;
					}				
				}
				
				zelki_biale_usegregowane.push_back(zelki_biale_ujecia[pozycja_biale]);
				zelki_zolte_usegregowane.push_back(zelki_zolte_ujecia[pozycja_zolte]);
				zelki_pomaranczowe_usegregowane.push_back(zelki_pomaranczowe_ujecia[pozycja_pomaranczowe]);
				zelki_zielone_usegregowane.push_back(zelki_zielone_ujecia[pozycja_zielone]);
				zelki_jasnoczerwone_usegregowane.push_back(zelki_jasnoczerwone_ujecia[pozycja_jasnoczerwone]);
				zelki_ciemnoczerwone_usegregowane.push_back(zelki_ciemnoczerwone_ujecia[pozycja_ciemnoczerwone]);
				
				biale_min = 100;
				zolte_min = 100;
				pomaranczowe_min = 100;
				zielone_min = 100;
				jasnoczerwone_min = 100;
				ciemnoczerwone_min = 100;

				zelki_biale_ujecia[pozycja_biale]=100;
				zelki_zolte_ujecia[pozycja_zolte]=100;
				zelki_pomaranczowe_ujecia[pozycja_pomaranczowe]=100;
				zelki_zielone_ujecia[pozycja_zielone]=100;
				zelki_jasnoczerwone_ujecia[pozycja_jasnoczerwone]=100;
				zelki_ciemnoczerwone_ujecia[pozycja_ciemnoczerwone]=100;
			}
			
			//median for objects found on every picture for current scene
			if (ilosc_ujec % 2 == 0){
				mediana_biale = (zelki_biale_usegregowane[ilosc_ujec / 2 - 1] + zelki_biale_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_biale = zelki_biale_usegregowane[ilosc_ujec / 2];
			if (ilosc_ujec % 2 == 0){
				mediana_zolte = (zelki_zolte_usegregowane[ilosc_ujec / 2 - 1] + zelki_zolte_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_zolte = zelki_zolte_usegregowane[ilosc_ujec / 2];
			if (ilosc_ujec % 2 == 0){
				mediana_pomaranczowe = (zelki_pomaranczowe_usegregowane[ilosc_ujec / 2 - 1] + zelki_pomaranczowe_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_pomaranczowe = zelki_pomaranczowe_usegregowane[ilosc_ujec / 2];
			if (ilosc_ujec % 2 == 0){
				mediana_zielone = (zelki_zielone_usegregowane[ilosc_ujec / 2 - 1] + zelki_zielone_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_zielone = zelki_zielone_usegregowane[ilosc_ujec / 2];
			if (ilosc_ujec % 2 == 0){
				mediana_jasnoczerwone = (zelki_jasnoczerwone_usegregowane[ilosc_ujec / 2 - 1] + zelki_jasnoczerwone_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_jasnoczerwone = zelki_jasnoczerwone_usegregowane[ilosc_ujec / 2];
			if (ilosc_ujec % 2 == 0){
				mediana_ciemnoczerwone = (zelki_ciemnoczerwone_usegregowane[ilosc_ujec / 2 - 1] + zelki_ciemnoczerwone_usegregowane[ilosc_ujec / 2]) / 2;
			}
			else mediana_ciemnoczerwone = zelki_ciemnoczerwone_usegregowane[ilosc_ujec / 2];
		
			cout << "biale: " << mediana_biale << endl;
			cout << "zolte: " << mediana_zolte<<endl;
			cout << "pomaranczowe: " << mediana_pomaranczowe << endl;
			cout << "zielone: " << mediana_zielone << endl;
			cout << "jasnoczerwone: " << mediana_jasnoczerwone << endl;
			cout << "ciemnoczerwone: " << mediana_ciemnoczerwone << endl;
			
			//saving results to txt
			plik_wyniki << mediana_ciemnoczerwone << ", " << mediana_jasnoczerwone << ", " << mediana_zielone << ", " << mediana_pomaranczowe << ", " << mediana_biale << ", " << mediana_zolte << endl;			
		}	
	}
	plik_wyniki.close();
	return 0;
}

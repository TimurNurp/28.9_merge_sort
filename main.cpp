/*
 - Доработайте алгоритм merge sort, описанный в 11 модуле, сделав его многопоточным.
 - Используйте std::futurez в качестве инструмента для синхронизации результата работы потоков.
 - Нужно определить независимые части алгоритма, которые могут быть распараллелены и запустить их вычисление в отдельных потоках, как это было сделано в юните 28.7.
 - Потоки могут быть запущены через std::thread или std::async, но синхронизация строго через std::future. Также предусмотрите ограничение количества запускаемых 
потоков по примеру из юнита 28.7.
*/

#include <future>
#include <iostream>
#include <chrono>
#include <random>

bool make_thread = true;

void merge(int* arr, int l, int m, int r)
{
   int nl = m - l + 1;
   int nr = r - m;

   // создаем временные массивы
   int left[nl], right[nr];

   // копируем данные во временные массивы
   for (int i = 0; i < nl; i++)
       left[i] = arr[l + i];
   for (int j = 0; j < nr; j++)
       right[j] = arr[m + 1 + j];

   int i = 0, j = 0;
   int k = l;  // начало левой части

   while (i < nl && j < nr) {
       // записываем минимальные элементы обратно во входной массив
       if (left[i] <= right[j]) {
           arr[k] = left[i];
           i++;
       }
       else {
           arr[k] = right[j];
           j++;
       }
       k++;
   }
   // записываем оставшиеся элементы левой части
   while (i < nl) {
       arr[k] = left[i];
       i++;
       k++;
   }
   // записываем оставшиеся элементы правой части
   while (j < nr) {
       arr[k] = right[j];
       j++;
       k++;
   }
}

void mergeSort(int* arr, int l, int r)
{
  if (l >= r)
    return;
  int m = (r-l-1)/2; //середина массива
  


  if(make_thread && (r - l > 10000))
   {
       // если элементов в левой части больше чем 10000
       // вызываем асинхронно рекурсию для правой части
       auto f = async(std::launch::async, [&](){
             mergeSort(arr, l, m);
       });
       
        mergeSort(arr, m+1, r);
   } else {
       // запускаем обе части синхронно
        mergeSort(arr, l, m);
        
        mergeSort(arr, m+1, r);
  }

  merge(arr, l, m, r); 
}

int main()
{
   srand(0);
   long arr_size = 100000000;
   int* array = new int[arr_size];  //создали массив

   for(long i=0; i<arr_size; i++) { //заполнили числами
       array[i] = rand() % 500000;
   }

   time_t start, end;


   // многопоточный запуск
   time(&start);
   mergeSort(array, 0, arr_size); //функция сортировки
   time(&end);

   double seconds = difftime(end, start);
   printf("The time: %f seconds\n", seconds);

   for(long i=0;i<arr_size-1; i++) { //проверка правильности сортировки
       if (array[i] > array[i + 1]) {
         std::cout << "Unsorted" << i << std::endl;
         break;
       }
   }



   for(long i=0;i<arr_size; i++) { //заполнили еще раз числами
       array[i] = rand() % 500000;
   }

   // однопоточный запуск
   make_thread = false;
   time(&start);
   mergeSort(array, 0, arr_size);
   time(&end);

   seconds = difftime(end, start);
   printf("The time: %f seconds\n", seconds);
   
   delete [] array;
   
   return 0;
}
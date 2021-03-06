(define revapp 
   (xs) 
   (lambda (ys) 
      (if (null? xs) ys ((revapp (cdr xs)) (cons (car xs) ys)))))
(define reverse (xs) ((revapp xs) '()))
(define revapp 
   (xs) 
   (lambda (ys) 
      (if (null? xs) ys ((revapp (cdr xs)) (cons (car xs) ys)))))
(define reverse (xs) ((revapp xs) '()))
(define get_field 
   (p?) 
   (lambda (xs) 
      (if (null? xs) 
         '() 
         (if (p? (car xs)) (cdr (car xs)) ((get_field p?) (cdr xs))))))
(define list1 (x) (cons x '()))
(define curriedLt (x) (lambda (y) (< x y)))
(define curriedGt (x) (lambda (y) (> x y)))
(define mkInsertionSort 
   (op?) 
   (letrec  ([insert 
      (lambda (x) 
         (lambda (xs) 
            (if (null? xs) 
               (list1 x) 
               (if ((op? x) (car xs)) 
                  (cons x xs) 
                  (cons (car xs) ((insert x) (cdr xs)))))))] [sort 
      (lambda (xs) 
         (if (null? xs) '() ((insert (car xs)) (sort (cdr xs)))))]) 
      sort))
(val sortIncreasing (mkInsertionSort curriedLt))
(val sortDecreasing (mkInsertionSort curriedGt))
(define listN (n) (if (= n 0) '() (cons n (listN (- n 1)))))
(val my1000List (listN 1000))
(val my10000List (listN 10000))
(check-expect (sortIncreasing '(3 2 1)) '(1 2 3))
(check-expect (sortIncreasing '(6 9 1 7 4 3 8 5 2 10)) 
   '(1 2 3 4 5 6 7 8 9 10))
(check-expect (sortDecreasing '(6 9 1 7 4 3 8 5 2 10)) 
   '(10 9 8 7 6 5 4 3 2 1))
(check-expect (sortIncreasing my1000List) (reverse my1000List))
(check-expect (sortIncreasing my10000List) (reverse my10000List))

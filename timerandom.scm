(define (timerandom max)
 (let ((num 0))
   (while (< num max)
;;	  (random)
	  (set! num (1+ num)))))

(timerandom 100000)

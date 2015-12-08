;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil (eval .
            (progn
              (setq ac-clang-flags
                    (mapcar (lambda (item)(concat "-I" item))
                            (split-string
                             "
/home/kongfy/Documents/Nanos/include/
/home/kongfy/Documents/Nanos/ring3/include/
")))
              (setq flycheck-clang-include-path
                    (split-string
                     "
/home/kongfy/Documents/Nanos/include/
/home/kongfy/Documents/Nanos/ring3/include/
"))))))

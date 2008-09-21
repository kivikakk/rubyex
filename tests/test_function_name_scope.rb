
def a(b)
  p b
end

def b(c, d=1)
  p [c, d]
end

def c(d, e=1, *f)
  p [d, e, f]
end

a(1)
b(1)
c(1)

b(1, 2)
c(1, 2)

c(1, 2, 3, 4, 5, 6, 7)

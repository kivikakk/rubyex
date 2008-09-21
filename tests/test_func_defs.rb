def a(b)
  p b
end

def b(c, d=:joe)
  p [c, d]
end

def c(d, e=:marly, *f)
  p [d, e, f]
end

a(9)
b(9)
c(9)

b('a', 'b')
c('a', 'b')

c(nil, false, true, 42, "John Citizen", 9.to_s + 4.to_s)


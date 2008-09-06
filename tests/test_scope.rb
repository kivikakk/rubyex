class A
  Z = 1
  class B
    Z = 2
    def m
      Z
    end
  end
end
Z = 3

p Z
p A::Z
p A::B::Z
p A::B.new.m


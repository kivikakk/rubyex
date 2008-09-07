module B
  class C
    def initialize
      puts "wrong class!"
    end
  end
end

p module A
  class B
    def initialize a, b
      @cc = "#{a.inspect} + #{b.inspect}"
    end

    def inspect
      to_s
    end

    def to_s
      @cc
    end
      
    class C
      def initialize a, b
	@b = B.new(a, b)
      end

      def joe
	p @b
      end
    end
  end

  B::C
end.new("some_class", :some_value).joe

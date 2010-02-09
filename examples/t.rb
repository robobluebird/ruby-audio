eval(File.read('load_samples.rb'))

@samples_to_graph = 500
lc = GoogleChart::LineChart.new('640x400', "Line Chart", false)
lc.data "First Sound", @f.to_a[0][0..@samples_to_graph], '0000ff'
lc.data "Last Sound", @l.to_a[0][0..@samples_to_graph], '00ff00'

lc.axis :y, :range => [[@f.min, @l.min].min, [@f.max,@l.max].max], :color => 'ff00ff', :font_size => 16, :alignment => :center
lc.axis :x, :range => [0,1000], :color => '00ffff', :font_size => 16, :alignment => :center
puts lc.to_url

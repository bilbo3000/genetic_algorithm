function visualizaiton()
m = csvread('data.csv');
figure, surf(m);
title('3D Visualization of Fitness Landscape');
xlabel('Chromosomes');
ylabel('Generations'); 
zlabel('Fitness');
print('-dpng', 'data.png');

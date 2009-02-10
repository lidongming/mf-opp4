clear; close all;

scalars=load('scalars.clean');
for i=1:length(scalars)/2,
tx(i)=scalars((i-1)*2+1);
acks(i)=scalars((i-1)*2+2);
end
PSR_sim=acks./tx;
load positions;
d=positions(2:length(positions));
set_params;

Lp=L0*d.^alpha;
Pr=Pt./(Lt*Lp*Lr);
%snir=Pr/N0/W;
%Pb_th=0.5*exp(-snir*8/Li);
Pb_th=ber_fsknc_snir(Pr/(N0*W*Li), 16); hold on;
% success if both data and ack packets transmitted error free
PSR_th=(1-Pb_th).^(ld+la);

figure(1);
semilogy(d, PSR_th); hold on;
semilogy(d, PSR_sim,'r-+'); hold on;
xlabel('distance [m]');
ylabel('DATA-ACK Success Rate');
legend('Theory','Simulation');
grid on;
axis([0 max(d) 1e-4 1]);

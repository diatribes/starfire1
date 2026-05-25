static const char *FRAG_SRC =
"#version 130\n"
"void main(){vec4 o=vec4(0);float i=0.,a,d=0.,s=0.,t=gl_Color.x*256.;vec2 r=gl_Color.yz*8192.;vec3 p;for(;i++<1e2;d+=s=.005+abs(s)*.5,o+=vec4(11,2.3-cos(.5*t),.8,0)/s)for(p=vec3(((gl_FragCoord.xy-r/2.)/r.y*(1.-cos(.5*t)*.05)+cos(t*.3)*vec2(.02,.03))*d,d-9.),s=length(p)-5.8,a=1.;a<24.;a+=a)p+=cos(.15*t+a+p.yzx*3.)*.3,s-=abs(dot(sin(.14*t+p*a*6.),.05+p-p))/a;gl_FragColor=tanh(o/1e4)*gl_Color.w;}";

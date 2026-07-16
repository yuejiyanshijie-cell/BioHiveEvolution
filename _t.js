
// ========== BioHive v4 - Red Alert Style ==========
const U = { // Units
  0:{n:'采集车',hp:300,dmg:0,spd:.15,cost:5,color:0xffaa00,size:.5,type:'harv'},
  1:{n:'步兵',hp:400,dmg:14,spd:.09,cost:20,color:0x00cc00,size:.65,type:'inf'},
  2:{n:'火箭兵',hp:280,dmg:32,spd:.07,cost:40,color:0xff2222,size:.8,type:'rocket'},
  3:{n:'狙击手',hp:150,dmg:60,spd:.08,cost:80,color:0xcc44ff,size:.6,type:'sniper'}
};
// Enemy types
const E = {
  basic:{n:'步兵',hp:300,dmg:5,spd:.05,clr:0xaa0000},
  tank:{n:'坦克',hp:600,dmg:8,spd:.03,clr:0x880000},
  fast:{n:'突击兵',hp:160,dmg:6,spd:.08,clr:0xff4400},
  boss:{n:'战车',hp:2000,dmg:12,spd:.04,clr:0x660000}
};

const S={
  started:false,over:false,mode:'observer',biomass:100,ate:0,crystalHP:2000,maxHP:2000,
  hiveHP:5000,maxHive:5000,hiveLast:5000,hivePhase:0,
  safety:30,waveNum:0,waveTimer:0,lastWave:0,placeType:0,
  units:[],enemies:[],ores:[],projectiles:[]
};

// Three.js
const C=document.getElementById('gc'),scene=new THREE.Scene();
scene.background=new THREE.Color(0x87ceeb);scene.fog=new THREE.Fog(0x87ceeb,150,400);
const camera=new THREE.PerspectiveCamera(70,innerWidth/innerHeight,.1,1000);
camera.position.set(0,30,60);
const R=new THREE.WebGLRenderer({canvas:C,antialias:true});
R.setSize(innerWidth,innerHeight);R.shadowMap.enabled=true;
scene.add(new THREE.AmbientLight(0xffffff,.7));
const dl=new THREE.DirectionalLight(0xffffcc,1.2);
dl.position.set(100,150,80);dl.castShadow=true;
dl.shadow.camera.left=-200;dl.shadow.camera.right=200;
dl.shadow.camera.top=200;dl.shadow.camera.bottom=-200;
scene.add(dl);

// World
(function(){
  const g=new THREE.Mesh(new THREE.PlaneGeometry(500,500),new THREE.MeshStandardMaterial({color:0x4a8a3a,roughness:.9}));
  g.rotation.x=-Math.PI/2;g.receiveShadow=true;scene.add(g);
  for(let i=0;i<300;i++){const x=(Math.random()-.5)*480,z=(Math.random()-.5)*480;
    const t=new THREE.Mesh(new THREE.CylinderGeometry(.25,.45,2.5,6),new THREE.MeshStandardMaterial({color:0x4a2f1b}));
    t.position.set(x,1.25,z);t.castShadow=true;scene.add(t);
    const l=new THREE.Mesh(new THREE.SphereGeometry(1.8,6,5),new THREE.MeshStandardMaterial({color:0x2d5a2d}));
    l.position.set(x,3.5,z);l.castShadow=true;scene.add(l);}
  for(let i=0;i<100;i++){const r=new THREE.Mesh(new THREE.DodecahedronGeometry(Math.random()*1.2+.6),new THREE.MeshStandardMaterial({color:0x666}));
    r.position.set((Math.random()-.5)*480,.5,(Math.random()-.5)*480);r.castShadow=true;scene.add(r);}
})();

// Crystal
const crystal=(()=>{
  const g=new THREE.Group();
  for(let l=0;l<3;l++){const m=new THREE.Mesh(new THREE.CylinderGeometry(5+l*1.5,5.5+l*1.5,.6,16),new THREE.MeshStandardMaterial({color:0x445566,roughness:.3,metalness:.7}));m.position.y=l*.6;m.castShadow=m.receiveShadow=true;g.add(m);}
  const c=new THREE.Mesh(new THREE.OctahedronGeometry(2.8,2),new THREE.MeshStandardMaterial({color:0x9944ff,roughness:.15,metalness:.9,emissive:0x331155,emissiveIntensity:.7}));c.position.y=3.5;c.castShadow=true;g.add(c);
  g.add(new THREE.Mesh(new THREE.SphereGeometry(.9,20,16),new THREE.MeshBasicMaterial({color:0xff66ff}))).position.y=3.5;
  for(let i=0;i<3;i++){const r=new THREE.Mesh(new THREE.TorusGeometry(3.5+i*1.2,.15,12,48),new THREE.MeshStandardMaterial({color:0xcc44ff,emissive:0x440088,emissiveIntensity:.5,roughness:.2}));r.position.y=3+i*1.5;r.userData={spd:.5+i*.3,axis:i%2?'z':'x'};g.add(r);}
  for(let i=0;i<6;i++){const a=(i/6)*Math.PI*2,p=new THREE.Mesh(new THREE.CylinderGeometry(.25,.35,5,8),new THREE.MeshStandardMaterial({color:0x665588,roughness:.2,emissive:0x110033,emissiveIntensity:.3}));p.position.set(Math.cos(a)*3.5,2.5,Math.sin(a)*3.5);p.castShadow=true;g.add(p);}
  const b=new THREE.Mesh(new THREE.CylinderGeometry(.4,.4,10,8),new THREE.MeshBasicMaterial({color:0xff44ff,transparent:true,opacity:.25}));b.position.y=10;g.add(b);
  for(let i=0;i<12;i++){const p=new THREE.Mesh(new THREE.SphereGeometry(.15,4,4),new THREE.MeshBasicMaterial({color:0xff88ff,transparent:true,opacity:.6}));p.position.y=5+i*.8;p.userData={by:5+i*.8,ph:i*.5};g.add(p);}
  g.position.set(0,0,0);scene.add(g);return g;
})();

// Enemy Hive
const hive=(()=>{
  const g=new THREE.Group();const cs=[0x660000,0x880000,0xaa0000];
  for(let i=0;i<3;i++){const c=new THREE.Mesh(new THREE.OctahedronGeometry(3+i*.8,2),new THREE.MeshStandardMaterial({color:cs[i],emissive:cs[i],emissiveIntensity:.5,roughness:.15,metalness:.8}));c.position.y=4+i*1.2;c.castShadow=true;c.rotation.y=i*Math.PI/3;g.add(c);}
  g.add(new THREE.Mesh(new THREE.SphereGeometry(1,20,16),new THREE.MeshBasicMaterial({color:0xff0000}))).position.y=6;
  for(let i=0;i<3;i++){const r=new THREE.Mesh(new THREE.TorusGeometry(4.5+i*1.5,.2,12,48),new THREE.MeshStandardMaterial({color:0xcc0000,emissive:0x440000,emissiveIntensity:.6,roughness:.2}));r.position.y=4+i*2;r.userData={spd:.3+i*.2,axis:i%2?'z':'x'};g.add(r);}
  for(let i=0;i<6;i++){const a=(i/6)*Math.PI*2,p=new THREE.Mesh(new THREE.CylinderGeometry(.3,.4,6,8),new THREE.MeshStandardMaterial({color:0x442222,emissive:0x110000,emissiveIntensity:.3}));p.position.set(Math.cos(a)*4.5,3,Math.sin(a)*4.5);p.castShadow=true;g.add(p);}
  g.position.set(0,0,-90);scene.add(g);return g;
})();

// Ore fields
for(let i=0;i<15;i++){
  const g=new THREE.Group();const x=(Math.random()-.5)*420,z=(Math.random()-.5)*420;
  for(let j=0;j<4;j++){
    const o=new THREE.Mesh(new THREE.OctahedronGeometry(.4+.2*j,1),new THREE.MeshStandardMaterial({color:0xffaa00,emissive:0x331100,emissiveIntensity:.3}));
    o.position.set(x+(j-1.5)*1.5,.3,z+Math.sin(j)*.8);g.add(o);
  }
  g.position.set(x,0,z);g.userData={amount:200+Math.random()*300};
  scene.add(g);S.ores.push(g);
}

// Cursor ring
const cursorMesh=new THREE.Mesh(new THREE.RingGeometry(1.5,2,32),new THREE.MeshBasicMaterial({color:0xffaa00,side:THREE.DoubleSide,transparent:true,opacity:.6}));
cursorMesh.rotation.x=-Math.PI/2;cursorMesh.visible=false;scene.add(cursorMesh);

// Camera state
let yaw=0,pitch=-0.5,observerPos=new THREE.Vector3(0,30,60),godCam=new THREE.Vector3(0,60,0),godHeight=60,cursorPos=new THREE.Vector3(),boxStart=null,boxActive=false;
// Input
const keys={};
window.addEventListener('resize',()=>{camera.aspect=innerWidth/innerHeight;camera.updateProjectionMatrix();R.setSize(innerWidth,innerHeight);});
document.addEventListener('keydown',e=>{keys[e.code]=true;if(e.code==='KeyQ')toggleMode();if(S.mode==='god'&&e.code==='Space'){e.preventDefault();placeUnit();}});
document.addEventListener('keyup',e=>{keys[e.code]=false;});
C.addEventListener('mousemove',e=>{if(!S.started)return;const r=C.getBoundingClientRect(),mx=(e.clientX-r.left)/r.width*2-1,my=-(e.clientY-r.top)/r.height*2+1;
  if(S.mode==='observer'&&document.pointerLockElement===C){yaw-=e.movementX*.002;pitch-=e.movementY*.002;pitch=Math.max(-Math.PI/3,Math.min(Math.PI/3,pitch));}
  if(S.mode==='god'){const rc=new THREE.Raycaster();rc.setFromCamera(new THREE.Vector2(mx,my),camera);const pt=new THREE.Vector3();rc.ray.intersectPlane(new THREE.Plane(new THREE.Vector3(0,1,0),0),pt);if(pt){pt.x=Math.max(-245,Math.min(245,pt.x));pt.z=Math.max(-245,Math.min(245,pt.z));cursorPos.copy(pt);cursorMesh.position.copy(pt).setY(.1);}}});
C.addEventListener('click',e=>{if(!S.started)return;if(S.mode==='observer')C.requestPointerLock();if(S.mode==='god')placeUnit();});
C.addEventListener('mousedown',e=>{if(S.mode!=='god'||!S.started||e.button!==0)return;if(!keys.ShiftLeft&&!keys.ShiftRight)return;boxStart=cursorPos.clone();boxActive=true;});
C.addEventListener('mouseup',e=>{if(!boxActive||!boxStart)return;boxActive=false;batchPlace(boxStart,cursorPos.clone());boxStart=null;});
C.addEventListener('wheel',e=>{if(S.mode!=='god')return;e.preventDefault();godHeight+=e.deltaY*.05;godHeight=Math.max(10,Math.min(150,godHeight));});

// UI
function msg(t,c){const m=document.getElementById('msg');m.textContent=t;m.style.color=c||'#0f0';m.classList.add('show');setTimeout(()=>m.classList.remove('show'),1500);}
function updUI(){document.getElementById('bio').textContent=Math.floor(S.biomass);document.getElementById('ate').textContent=S.ate;document.getElementById('cnt').textContent=S.units.length;document.getElementById('cry').textContent=Math.max(0,Math.ceil(S.crystalHP))+'/'+S.maxHP;document.getElementById('hive').textContent=Math.max(0,Math.ceil(S.hiveHP))+'/'+S.maxHive;}

// Unit placement
function pick(n){S.placeType=n;[0,1,2,3].forEach(i=>{const b=document.getElementById('s'+i);if(b)b.className=i===n?'sel':'';});const cs=[0xffaa00,0x00cc00,0xff2222,0xcc44ff];cursorMesh.material.color.set(cs[n]);}
function placeUnit(){if(S.mode!=='god')return;spawn(S.placeType,cursorPos.x,cursorPos.z);updUI();cursorMesh.material.color.set(0xffffff);setTimeout(()=>pick(S.placeType),200);}
function batchPlace(a,b){const cols=4,rows=3,total=cols*rows,cost=U[S.placeType].cost*total;if(S.biomass<cost){msg('需要'+cost+'资金');return;}const mx=Math.min(a.x,b.x),Mx=Math.max(a.x,b.x),mz=Math.min(a.z,b.z),Mz=Math.max(a.z,b.z),sx=(Mx-mx)/(cols-1),sz=(Mz-mz)/(rows-1);for(let r=0;r<rows;r++)for(let c=0;c<cols;c++)spawn(S.placeType,mx+c*sx,mz+r*sz);updUI();msg('批量放置'+total+'个!');}

// Spawn unit
function spawn(type,x,z){
  const d=U[type],cost=d.cost;if(S.biomass<cost){msg('资金不足');return;}S.biomass-=cost;
  const g=new THREE.Group();
  // Distinct shapes per unit type
  if(type===0){// Harvester - tank shape
    const b=new THREE.Mesh(new THREE.BoxGeometry(.8,.5,1.2),new THREE.MeshStandardMaterial({color:d.color}));b.position.y=.4;b.castShadow=true;g.add(b);
    const t=new THREE.Mesh(new THREE.CylinderGeometry(.2,.2,.3,6),new THREE.MeshStandardMaterial({color:0xaaaa00}));t.position.set(0,.7,0);g.add(t);
  }else if(type===1){// Infantry
    const b=new THREE.Mesh(new THREE.CylinderGeometry(.35,.4,1.3,8),new THREE.MeshStandardMaterial({color:d.color}));b.position.y=.65;b.castShadow=true;g.add(b);
    const h=new THREE.Mesh(new THREE.SphereGeometry(.3,6,4),new THREE.MeshStandardMaterial({color:0xe8c4a0}));h.position.y=1.5;g.add(h);
    const gun=new THREE.Mesh(new THREE.CylinderGeometry(.05,.05,.6,6),new THREE.MeshStandardMaterial({color:0x333}));gun.position.set(.4,1,0);gun.rotation.z=-Math.PI/4;g.add(gun);
  }else if(type===2){// Rocket soldier
    const b=new THREE.Mesh(new THREE.CylinderGeometry(.4,.5,1.4,8),new THREE.MeshStandardMaterial({color:d.color}));b.position.y=.7;b.castShadow=true;g.add(b);
    const h=new THREE.Mesh(new THREE.SphereGeometry(.35,6,4),new THREE.MeshStandardMaterial({color:0xe8c4a0}));h.position.y=1.6;g.add(h);
    const rl=new THREE.Mesh(new THREE.CylinderGeometry(.08,.12,.8,6),new THREE.MeshStandardMaterial({color:0x444}));rl.position.set(.55,1.1,.3);rl.rotation.z=-Math.PI/3;g.add(rl);
  }else{// Sniper
    const b=new THREE.Mesh(new THREE.CylinderGeometry(.3,.35,1.5,8),new THREE.MeshStandardMaterial({color:d.color}));b.position.y=.75;b.castShadow=true;g.add(b);
    const h=new THREE.Mesh(new THREE.SphereGeometry(.3,6,4),new THREE.MeshStandardMaterial({color:0xe8c4a0}));h.position.y=1.7;g.add(h);
    const barrel=new THREE.Mesh(new THREE.CylinderGeometry(.03,.04,1,6),new THREE.MeshStandardMaterial({color:0x222}));barrel.position.set(.35,1.4,0);barrel.rotation.z=-Math.PI/3;g.add(barrel);
  }
  g.position.set(x,0,z);
  g.userData={type,hp:d.hp,maxHp:d.hp,dmg:d.dmg,spd:d.spd,lastAttack:0,kills:0,mining:false,miningTimer:0,hasOre:false};
  scene.add(g);S.units.push(g);
}

// Spawn enemy
function spawnEnemy(type){
  const d=E[type];const g=new THREE.Group();
  if(type==='boss'){const b=new THREE.Mesh(new THREE.BoxGeometry(1.5,.8,2),new THREE.MeshStandardMaterial({color:d.clr}));b.position.y=.6;b.castShadow=true;g.add(b);g.scale.set(2,2,2);}
  else{const b=new THREE.Mesh(new THREE.ConeGeometry(.6,1.5,6),new THREE.MeshStandardMaterial({color:d.clr,roughness:.8}));b.position.y=.75;b.castShadow=true;g.add(b);}
  g.userData={type,hp:d.hp,maxHp:d.hp,dmg:d.dmg,spd:d.spd,lastAttack:0};
  S.enemies.push(g);scene.add(g);return g;
}

// Toggle mode
function toggleMode(){
  if(S.mode==='observer'){S.mode='god';if(document.exitPointerLock)document.exitPointerLock();cursorMesh.visible=true;document.getElementById('shop').style.display='block';godCam.set(cursorPos.x||0,godHeight,cursorPos.z||0);pick(S.placeType);}
  else{S.mode='observer';cursorMesh.visible=false;document.getElementById('shop').style.display='none';C.requestPointerLock();}
}

// Squads
let squads=[];
function createSquad(){
  const sq={units:[],id:Date.now()+Math.random()};
  const sx=hive.position.x+(Math.random()-.5)*50,sz=hive.position.z+(Math.random()-.5)*25+12,sp=5;
  if(S.waveNum>=4){const b=spawnEnemy('boss');b.position.set(sx,0,sz);b.userData.fx=0;b.userData.fz=-sp;b.userData.sid=sq.id;sq.units.push(b);}
  const rc=S.waveNum<=1?2:S.waveNum<=3?3:4,cc=S.waveNum<=1?3:5,ft=S.waveNum>=2?'tank':'basic';
  for(let r=0;r<rc;r++){const tp=r===0?ft:'basic';for(let c=0;c<cc;c++){const e=spawnEnemy(tp);e.position.set(sx+(c-(cc-1)/2)*sp,0,sz+(r+1)*sp);e.userData.fx=(c-(cc-1)/2)*sp;e.userData.fz=(r+1)*sp;e.userData.sid=sq.id;sq.units.push(e);}}
  if(S.waveNum>=3){for(let s=-1;s<=1;s+=2)for(let i=0;i<2;i++){const e=spawnEnemy('fast');e.position.set(sx+s*(sp*3+i*3),0,sz+i*sp+sp);e.userData.fx=s*(sp*3+i*3);e.userData.fz=i*sp+sp;e.userData.sid=sq.id;sq.units.push(e);}}
  squads.push(sq);
}
function launchWave(){
  const now=Date.now(),cooldown=S.waveNum<2?25000:S.waveNum<4?22000:18000;
  if(now-S.lastWave<cooldown&&S.waveNum>0)return;S.lastWave=now;S.waveNum++;
  msg('第 '+S.waveNum+' 波来袭!','#f00');
  const cnt=S.waveNum<=1?3:S.waveNum<=3?4:5;
  for(let i=0;i<cnt;i++)setTimeout(()=>{const sq=createSquad();const mult=S.waveNum<=5?S.waveNum-1:(S.waveNum-5)*2+4,hs=1+mult*.15,as=1+mult*.08,ss=1+mult*.04;sq.units.forEach(u=>{u.userData.hp=Math.floor(u.userData.hp*hs);u.userData.maxHp=u.userData.hp;u.userData.dmg=Math.floor(u.userData.dmg*as);if(u.userData.type==='boss')u.scale.setScalar(ss*2);else u.scale.setScalar(ss);});},i*800);
}

// Process kill
function processKill(u,n){S.biomass+=30;S.ate++;u.userData.kills++;scene.remove(n);S.enemies.splice(S.enemies.indexOf(n),1);}
function gameOver(){document.getElementById('fk').textContent=S.ate;document.getElementById('fb').textContent=Math.floor(S.biomass);document.getElementById('over').style.display='flex';}

// Game loop
function loop(time){
  requestAnimationFrame(loop);if(!S.started||S.over){R.render(scene,camera);return;}
  const dt=Math.min(.05,time/1000-(loop._lt||0));loop._lt=time/1000;
  const now=Date.now();

  // Safety
  if(S.safety>0){S.safety-=dt;const el=document.getElementById('safety');if(S.safety<=0){S.safety=0;el.style.display='none';msg('敌军来袭!','#f44');launchWave();}else{el.style.display='block';el.textContent='安全期 '+Math.ceil(S.safety)+'s';el.style.color=S.safety<5?'#f44':'#ff0';}}

  // Observer camera
  if(S.mode==='observer'){const d=new THREE.Vector3();if(keys.KeyW)d.z-=1;if(keys.KeyS)d.z+=1;if(keys.KeyA)d.x-=1;if(keys.KeyD)d.x+=1;if(d.length()>0){d.normalize().applyAxisAngle(new THREE.Vector3(0,1,0),yaw);observerPos.x+=d.x*dt*40;observerPos.z+=d.z*dt*40;}if(keys.KeyQ)observerPos.y+=dt*30;if(keys.KeyE)observerPos.y-=dt*30;observerPos.y=Math.max(5,Math.min(200,observerPos.y));observerPos.x=Math.max(-245,Math.min(245,observerPos.x));observerPos.z=Math.max(-245,Math.min(245,observerPos.z));camera.position.lerp(observerPos,.08);const lk=observerPos.clone();lk.x+=Math.sin(yaw)*15;lk.z+=Math.cos(yaw)*15;lk.y-=5;camera.lookAt(lk);}

  // God camera
  if(S.mode==='god'){const ps=dt*50;if(keys.KeyW)godCam.z-=ps;if(keys.KeyS)godCam.z+=ps;if(keys.KeyA)godCam.x-=ps;if(keys.KeyD)godCam.x+=ps;godCam.x=Math.max(-245,Math.min(245,godCam.x));godCam.z=Math.max(-245,Math.min(245,godCam.z));camera.position.set(godCam.x,godHeight,godCam.z+.01);camera.lookAt(godCam.x,0,godCam.z);}

  // Enemy squads march
  squads.forEach(sq=>{const alive=sq.units.filter(u=>u.userData.hp>0);if(!alive.length)return;let cx=0,cz=0;alive.forEach(u=>{cx+=u.position.x;cz+=u.position.z});cx/=alive.length;cz/=alive.length;const td=Math.sqrt(cx*cx+cz*cz),dx=td>.1?-cx/td:0,dz=td>.1?-cz/td:0,spd=5*dt;
    alive.forEach(u=>{const fx=u.userData.fx||0,fz=u.userData.fz||0,ang=Math.atan2(dx,dz),rx=fx*Math.cos(-ang)-fz*Math.sin(-ang),rz=fx*Math.sin(-ang)+fz*Math.cos(-ang),mx=cx+rx,mz=cz+rz,edx=mx-u.position.x,edz=mz-u.position.z,ed=Math.sqrt(edx*edx+edz*edz),s=spd*(u.userData.type==='fast'?1.4:1);if(ed>1){u.position.x+=(edx/ed)*s;u.position.z+=(edz/ed)*s;}if(td<50){let tgt=null,md=5;S.units.forEach(un=>{const d=u.position.distanceTo(un.position);if(d<md&&un.userData.hp>0){md=d;tgt=un;}});if(!tgt&&u.position.distanceTo(crystal.position)<6)tgt=crystal;if(tgt&&now-(u.userData.lastAttack||0)>1200){u.userData.lastAttack=now;if(tgt===crystal){S.crystalHP-=u.userData.dmg;if(S.crystalHP<=0){S.crystalHP=0;S.over=true;gameOver();}}else{tgt.userData.hp-=u.userData.dmg;if(tgt.userData.hp<=0){scene.remove(tgt);S.units.splice(S.units.indexOf(tgt),1);}}}}u.position.y=0;})});squads=squads.filter(s=>s.units.some(u=>u.userData.hp>0));

  // Player unit AI
  S.units.forEach(u=>{const tp=u.userData.type,s=u.userData.spd*dt*60;
    if(tp===0){if(u.userData.hasOre){const dc=u.position.distanceTo(crystal.position);if(dc<5){u.userData.hasOre=false;u.userData.mining=false;S.biomass+=15;updUI();}else{const dv=new THREE.Vector3().subVectors(crystal.position,u.position).normalize();u.position.x+=dv.x*s;u.position.z+=dv.z*s;}}else{let nearest=null,nd=Infinity;S.ores.forEach(o=>{const d=u.position.distanceTo(o.position);if(d<nd){nd=d;nearest=o;}});if(nearest){const dv=new THREE.Vector3().subVectors(nearest.position,u.position).normalize();u.position.x+=dv.x*s;u.position.z+=dv.z*s;if(nd<3){u.userData.miningTimer=(u.userData.miningTimer||0)+dt;if(u.userData.miningTimer>2){u.userData.miningTimer=0;u.userData.hasOre=true;nearest.userData.amount-=15;if(nearest.userData.amount<=0){const nx=(Math.random()-.5)*420,nz=(Math.random()-.5)*420;nearest.position.set(nx,0,nz);nearest.userData.amount=200+Math.random()*300;}}}}}}
    }else{const isRanged=tp===2||tp===3,range=isRanged?(tp===3?28:20):4;let n=null,nd=50;S.enemies.forEach(e=>{const d=u.position.distanceTo(e.position);if(d<nd&&e.userData.hp>0){nd=d;n=e;}});if(n){const dv=new THREE.Vector3().subVectors(n.position,u.position).normalize();if(nd>range+2){u.position.x+=dv.x*s;u.position.z+=dv.z*s;}if(nd<range&&now-(u.userData.lastAttack||0)>800){u.userData.lastAttack=now;if(isRanged){const p=new THREE.Mesh(new THREE.SphereGeometry(.15,4,4),new THREE.MeshBasicMaterial({color:tp===3?0xcc44ff:0xff2222}));p.position.set(u.position.x,1,u.position.z);p.userData={dmg:u.userData.dmg,owner:u,target:n,vx:dv.x*16,vz:dv.z*16,life:2};scene.add(p);S.projectiles.push(p);}else{n.userData.hp-=u.userData.dmg;if(n.userData.hp<=0)processKill(u,n);}}}else{const dh=u.position.distanceTo(hive.position);if(dh<(isRanged?15:8)&&now-(u.userData.lastAttack||0)>500){u.userData.lastAttack=now;if(isRanged){const dhv=new THREE.Vector3().subVectors(hive.position,u.position).normalize();const p=new THREE.Mesh(new THREE.SphereGeometry(.15,4,4),new THREE.MeshBasicMaterial({color:0xff0000}));p.position.set(u.position.x,1,u.position.z);p.userData={dmg:u.userData.dmg,owner:u,target:hive,isHive:true,vx:dhv.x*12,vz:dhv.z*12,life:2};scene.add(p);S.projectiles.push(p);}else{S.hiveHP-=u.userData.dmg;if(S.hiveHP<=0){S.hiveHP=0;S.over=true;gameWin();}}}else if(dh<40&&!isRanged){const dhv=new THREE.Vector3().subVectors(hive.position,u.position).normalize();u.position.x+=dhv.x*s;u.position.z+=dhv.z*s;}}}u.position.y=0;});

  // Projectiles
  S.projectiles=S.projectiles.filter(p=>{p.position.x+=p.userData.vx*dt;p.position.z+=p.userData.vz*dt;p.userData.life-=dt;if(p.userData.life<=0){scene.remove(p);return false;}const tgt=p.userData.target;if(tgt&&p.position.distanceTo(p.userData.isHive?tgt.position:tgt.position)<1.5){if(p.userData.isHive){S.hiveHP-=p.userData.dmg;if(S.hiveHP<=0){S.hiveHP=0;S.over=true;gameWin();}}else{tgt.userData.hp-=p.userData.dmg;if(tgt.userData.hp<=0)processKill(p.userData.owner,tgt);}scene.remove(p);return false;}return true;});

  // Wave timer
  if(S.safety<=0){S.waveTimer+=dt;if(S.waveTimer>=20){S.waveTimer=0;if(squads.length<10)launchWave();}}

  // Hive phase + damage spawn
  const hpct=S.hiveHP/S.maxHive,prevPhase=S.hivePhase;
  if(hpct<=.1)S.hivePhase=4;else if(hpct<=.25)S.hivePhase=3;else if(hpct<=.5)S.hivePhase=2;else if(hpct<=.75)S.hivePhase=1;
  if(S.hivePhase>prevPhase){const lvl=S.hivePhase;const gs=lvl===1?[[3,'tank'],[5,'basic']]:lvl===2?[[5,'tank'],[8,'basic'],[2,'fast'],[1,'boss']]:lvl===3?[[8,'tank'],[12,'basic'],[4,'fast'],[2,'boss']]:[[12,'tank'],[18,'basic'],[6,'fast'],[3,'boss']];gs.forEach(([n,tp])=>{for(let i=0;i<n;i++){const g=spawnEnemy(tp);g.position.set(hive.position.x+(Math.random()-.5)*20,0,hive.position.z+(Math.random()-.5)*15+15);g.userData.hp=Math.floor(g.userData.hp*(1+lvl*.5));g.userData.maxHp=g.userData.hp;g.userData.dmg=Math.floor(g.userData.dmg*(1+lvl*.3));}});msg('母巢第'+lvl+'阶段!','#f00');}
  const dmgTaken=S.hiveLast-S.hiveHP;if(dmgTaken>20){const sc=Math.min(4,Math.floor(dmgTaken/50)+1);for(let i=0;i<sc;i++){const e=spawnEnemy(['basic','basic','tank','fast'][Math.floor(Math.random()*4)]);e.position.set(hive.position.x+(Math.random()-.5)*15,0,hive.position.z+(Math.random()-.5)*10+10);}}S.hiveLast=S.hiveHP;
  const hc=hive.children[1];if(hc&&hc.material)hc.material.color.set(hpct>.5?0xff0000:hpct>.25?0xff4400:0xff0000);
  hive.children.forEach(ch=>{if(ch.userData&&ch.userData.spd>=0){if(ch.userData.axis==='x')ch.rotation.x+=.01*ch.userData.spd;else ch.rotation.z+=.01*ch.userData.spd;}});

  // Crystal animation
  crystal.children.forEach(ch=>{if(ch.userData&&ch.userData.spd>=0){if(ch.userData.axis==='x')ch.rotation.x+=.01*ch.userData.spd;else ch.rotation.z+=.01*ch.userData.spd;}if(ch.userData&&ch.userData.by!==undefined)ch.position.y=ch.userData.by+Math.sin(time/500+ch.userData.ph)*.5;});
  const cc=crystal.children[4];if(cc&&cc.material){const pct=S.crystalHP/S.maxHP;cc.material.color.set(pct>.5?0xff66ff:pct>.25?0xff8844:0xff2222);}
  updUI();R.render(scene,camera);
}

function gameWin(){S.over=true;msg('胜利! 母巢已摧毁','#ffd700');document.getElementById('over').querySelector('h1').textContent='⚔ 胜利!';document.getElementById('over').style.display='flex';}

// Start
function startGame(){document.getElementById('start').style.display='none';S.started=true;S.safety=30;C.requestPointerLock();loop(0);}

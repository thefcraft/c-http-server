const input = document.getElementById('input');
const input_blink = document.getElementById('input_blink');
window.setInterval(()=>{
    if (input_blink.innerHTML=='') input_blink.innerHTML = '|';
    else input_blink.innerHTML = '';
}, 500)
function btnHandler(ele){
    console.log(ele);
}

function inpBtnHandler(ele, value){
    input.innerHTML += value;
    console.log(value);
}

window.onkeydown = (e)=>{
    inpBtnHandler(null, e.key);
}
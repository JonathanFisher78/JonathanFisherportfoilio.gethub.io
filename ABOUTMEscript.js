
function goToPage(page) {
    var element = document.getElementById(page);
    if (element) {
        element.scrollIntoView({ behavior: 'smooth' });
    }
}
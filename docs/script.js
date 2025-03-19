// DataFrame Documentation JavaScript

document.addEventListener('DOMContentLoaded', function() {
  // Initialize animations
  animateElements();
  
  // Initialize modals
  initializeModals();
  
  // Initialize search
  initializeSearch();
  
  // Initialize copy buttons
  initializeCopyButtons();
});

// Animate elements on page load
function animateElements() {
  const fadeElements = document.querySelectorAll('.animate-fade-in');
  const slideElements = document.querySelectorAll('.animate-slide-up');
  
  fadeElements.forEach(el => {
    el.style.opacity = '0';
    setTimeout(() => {
      el.style.opacity = '1';
      el.classList.add('animated');
    }, 100);
  });
  
  slideElements.forEach(el => {
    el.style.opacity = '0';
    el.style.transform = 'translateY(20px)';
    setTimeout(() => {
      el.style.opacity = '1';
      el.style.transform = 'translateY(0)';
      el.classList.add('animated');
    }, 100 + (parseInt(el.dataset.delay || 0)));
  });
}

// Initialize modal functionality
function initializeModals() {
  const modalOverlay = document.querySelector('.modal-overlay');
  const modalCloseButtons = document.querySelectorAll('.modal-close');
  const cards = document.querySelectorAll('.card');
  
  if (!modalOverlay) return;
  
  // Open modal when a card is clicked
  cards.forEach(card => {
    card.addEventListener('click', () => {
      const functionId = card.dataset.function;
      const modalContent = document.getElementById(`modal-${functionId}`);
      
      if (modalContent) {
        // Clone the content and append it to the modal body
        const modalBody = document.querySelector('.modal-body');
        modalBody.innerHTML = '';
        modalBody.appendChild(modalContent.content.cloneNode(true));
        
        // Set the modal title
        document.querySelector('.modal-title').textContent = card.querySelector('.card-title').textContent;
        
        // Show the modal
        modalOverlay.classList.add('active');
        document.body.style.overflow = 'hidden';
      }
    });
  });
  
  // Close modal when the close button is clicked
  modalCloseButtons.forEach(button => {
    button.addEventListener('click', () => {
      modalOverlay.classList.remove('active');
      document.body.style.overflow = '';
    });
  });
  
  // Close modal when clicking outside the modal
  modalOverlay.addEventListener('click', (e) => {
    if (e.target === modalOverlay) {
      modalOverlay.classList.remove('active');
      document.body.style.overflow = '';
    }
  });
  
  // Close modal when pressing Escape key
  document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && modalOverlay.classList.contains('active')) {
      modalOverlay.classList.remove('active');
      document.body.style.overflow = '';
    }
  });
}

// Initialize search functionality
function initializeSearch() {
  const searchBox = document.querySelector('.search-box');
  if (!searchBox) return;
  
  searchBox.addEventListener('input', (e) => {
    const searchTerm = e.target.value.toLowerCase();
    const cards = document.querySelectorAll('.card');
    
    cards.forEach(card => {
      const title = card.querySelector('.card-title').textContent.toLowerCase();
      const description = card.querySelector('.card-description').textContent.toLowerCase();
      const tags = Array.from(card.querySelectorAll('.card-tag')).map(tag => tag.textContent.toLowerCase());
      
      const isMatch = title.includes(searchTerm) || 
                      description.includes(searchTerm) || 
                      tags.some(tag => tag.includes(searchTerm));
      
      card.style.display = isMatch ? 'block' : 'none';
    });
    
    // Check if any category is now empty
    const categories = document.querySelectorAll('.category-container');
    categories.forEach(category => {
      const visibleCards = category.querySelectorAll('.card[style="display: block"]');
      category.style.display = visibleCards.length > 0 ? 'block' : 'none';
    });
  });
}

// Initialize copy buttons for code blocks
function initializeCopyButtons() {
  const copyButtons = document.querySelectorAll('.copy-button');
  
  copyButtons.forEach(button => {
    button.addEventListener('click', () => {
      const codeBlock = button.closest('.code-container').querySelector('pre code');
      const textToCopy = codeBlock.textContent;
      
      navigator.clipboard.writeText(textToCopy).then(() => {
        const originalText = button.textContent;
        button.textContent = 'Copied!';
        
        setTimeout(() => {
          button.textContent = originalText;
        }, 2000);
      });
    });
  });
}

// Function to open a specific documentation page
function openDocPage(functionName) {
  window.location.href = `details/${functionName}.html`;
}

// Function to highlight related functions when hovering
function initializeRelatedFunctions() {
  const relatedFunctions = document.querySelectorAll('.related-function');
  
  relatedFunctions.forEach(func => {
    func.addEventListener('mouseenter', () => {
      func.classList.add('highlight');
    });
    
    func.addEventListener('mouseleave', () => {
      func.classList.remove('highlight');
    });
  });
}

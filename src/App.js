import React, { useState } from 'react';
import { css, cx } from 'emotion'

const windowClass = css`
  width: 100vw;
  max-width: 600px;
  height: 80vh;
  margin: 0 auto;
  overflow: auto;
`;

const titleClass = css`
  text-align: center;
  padding: 0.1em;
  font-size: 2em;
`;

const rowClass = css`
  position: relative;
  padding: 1px 0;

  .row-controls {
    display: none;
  }
  &:hover .row-controls {
    display: flex;
  }
  &:focus .row-controls {
    display: flex;
  }
  &:active .row-controls {
    display: flex;
  }
`;
const cellClass = css`
  width: 33%;
  display: inline-block;
`;


const filterClass = css`
  padding: 5px 2px;
  font-size: 16px;
  display: block;
  margin: 0 auto 5px;
`
const countClass = css`
  font-size: 10px;
`

const rowControlsClass = css`
  display: flex;
  position: absolute;
  width: 100px;
  align-items: center;
  justify-content: space-around;
  font-size: 12px;
  top: 0px;
  right: 0px;
  z-index: 0;
  
  span {
    background-color: white;
    box-shadow: 2px 2px black;
    border-radius: 20px;
    padding: 5px;

  }
`;

const commitRowActionClass = css`
  display: inline-flex;
  align-items: center;
  justify-content: space-between;
`;

const commitRowActionCancelClass = css`
  font-weight: bold;
`

const commitRowActionConfirmClass = css`
  font-weight: bold;
`

const startDeleteClass = css`
  color: #ff3232;
`

const slidingNewFormClass = isOpen => css`
  display:inline-block;
  max-width: 0;
  opacity: 0;
  overflow: hidden;
  ${isOpen && 'max-width: 400px;'}
  ${isOpen && 'opacity: 1;'}
  transition: max-width 0.2s ease;
`;

const APP_DOMAIN = window.location.origin;

const deleteItem = async (
  domain,
  setBlacklist,
  setToast,
) => {
  const response = await fetch(APP_DOMAIN, {
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({domain}),
    method: 'DELETE',
  })
  const jsonResponse = await response.json();
  console.log("DELETE REQUEST RETURNS")
  console.log(jsonResponse)
}

const updateItem = async (
  editingItem,
  setBlacklist,
  setToast,
) => {
  const response = await fetch(APP_DOMAIN, {
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(editingItem),
    method: 'PUT',
  })
  const jsonResponse = await response.json();
  console.log("PUT REQUEST RETURNS")
  console.log(jsonResponse)
}

const addItem = async (
  newItem,
  setBlacklist,
  setToast,
) => {
  const response = await fetch(APP_DOMAIN, {
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(newItem),
    method: 'POST',
  })
  const jsonResponse = await response.json();
  console.log("POST REQUEST RETURNS")
  console.log(jsonResponse)
}

function getFormattedDate(date) {
  var year = date.getFullYear();

  var month = (1 + date.getMonth()).toString();
  month = month.length > 1 ? month : '0' + month;

  var day = date.getDate().toString();
  day = day.length > 1 ? day : '0' + day;
  
  return month + '/' + day + '/' + year;
}

const NEW_EMPTY_ITEM = {
  domain: '',
  redirect: ''
}
const EMPTY_TOASTS = []
const TOAST_TYPES = {
  SUCCESS: 'SUCCESS',
  FAILURE: 'FAILURE',
  WARNING: 'WARNING',
  INFO: 'INFO',
};

const TOAST_TIMES = {
  [TOAST_TYPES.SUCCESS]: 15 * 1000,
  [TOAST_TYPES.FAILURE]: 120 * 1000,
  [TOAST_TYPES.WARNING]: 60 * 1000,
  [TOAST_TYPES.INFO]: 15 * 1000,
};

function App() {
  const [blacklist, setBlacklist] = useState(JSON.parse(JSON.stringify(window.INITIAL_BLACKLIST || {})))
  const [editingItem, setEditingItem] = useState(null);
  const [newItem, setNewItem] = useState(NEW_EMPTY_ITEM);
  const [newItemOpen, setNewItemOpen] = useState(false);
  const [deleteToConfirm, setDeleteToConfirm] = useState(null);
  const [searchFilter, setSearchFilter] = useState('');
  const [toasts, setToasts] = useState(EMPTY_TOASTS);
  const resetUserState = () => {
    setEditingItem(null);
    setNewItem(NEW_EMPTY_ITEM);
    setDeleteToConfirm(null);
    setSearchFilter('');
    setNewItemOpen(false);
  }

  const blacklistDomains = Object.keys(blacklist);
  const domainsToShow = searchFilter
    ? blacklistDomains.filter(x => {
      return x.includes(searchFilter) ||
        blacklist[x].redirect.includes(searchFilter) 
    }) 
    : blacklistDomains
  const countVerbiage =  domainsToShow.length < blacklistDomains.length
    ? `showing ${domainsToShow.length} of ${blacklistDomains.length}`
    : `${domainsToShow.length} domains`

  return (
    <div>
      <header className={titleClass}>
        Mascarpone internet filterer
        <span className={countClass}>
          ({countVerbiage}) 
        </span>
      </header>
      <input placeholder="Find a filtered domain" className={filterClass} onChange={(e) => {
        setSearchFilter(e.target.value);
        if (deleteToConfirm) {
          setDeleteToConfirm(null);
        }
        if (newItem.domain || newItem.redirect) {
          setNewItem(NEW_EMPTY_ITEM);
        }
        if (editingItem) {
          setEditingItem(null);
        }
      }}/>
      <div className={windowClass}>
        <div className={rowClass}>
          <div className={cellClass}>Domain</div>
          <div className={cellClass}>Go to</div>
          <div className={cellClass}>Date updated</div>
        </div>
        {
          domainsToShow.map(domain => {
            const redirect = blacklist[domain].redirect;
            if (deleteToConfirm === domain) {
              return <DeleteRow
                key={domain}
                domain={domain}
                redirect={redirect}
                confirm={() => deleteItem(domain, setBlacklist, () => {console.log('set toast here')})}
                cancel={() => setDeleteToConfirm(null)}
              />
            } else if (editingItem && editingItem.domain === domain) {
              return <EditingRow
                key={domain}
                editingItem={editingItem}
                cancel={
                  () => setEditingItem(null)
                }
                confirm={
                  () => {
                    updateItem(editingItem, (newBlacklist, message) => {
                      setBlacklist(newBlacklist)
                      if (message) {
                        console.log("TODO: TOAST USER")
                      }
                      resetUserState()
                    })
                  }
                }
                updateEditingItem={
                  (partialUpdate) => {
                    setEditingItem({
                      ...editingItem,
                      ...partialUpdate,
                    })
                  }
                }
              />
            }
            return <Row 
            key={domain}
            domain={domain}
            redirect={redirect}
            updated={(new Date(blacklist[domain].updated * 1000)).toLocaleString()}
            setDeleteToConfirm={setDeleteToConfirm}
            setEditingItem={setEditingItem}
            canShowControls={!deleteToConfirm && !editingItem}
          />
          })
        }
      </div>
      <NewRow
        newItem={newItem}
        setNewItem={setNewItem}
        open={() => setNewItemOpen(true)}
        cancel={() => {
          setNewItem(NEW_EMPTY_ITEM)
          setNewItemOpen(false)
        }}
        confirm={() => {
          addItem(newItem)
        }}
        isOpen={newItemOpen}
      />
    </div>
  );
}

const EditingRow = ({
  editingItem,
  confirm,
  cancel,
  updateEditingItem,
}) => {
  const {
    newDomain,
    redirect,
  } = editingItem
  return <div tabIndex={0} className={rowClass}>
    <div className={cellClass}>
      <input
        onChange={e => { updateEditingItem({newDomain: e.target.value })}}
        placeholder="New domain"
        value={newDomain}
      />
    </div>
    <div className={cellClass}>
      <input
        onChange={e => { updateEditingItem({redirect: e.target.value})}}
        placeholder="New Redirect"
        value={redirect}
      />
    </div>
    <CommitRowAction cancel={cancel} confirm={confirm}/>
  </div>
}

const NewRow = ({
  confirm,
  cancel,
  open,
  isOpen,
  newItem,
  setNewItem,
}) => {
  const {
    domain,
    redirect,
  } = newItem;
  return (
    <div>
      <div className={slidingNewFormClass(isOpen)}>
        <input placeholder="New domain" value={domain} onChange={e => {
          setNewItem({
            domain: e.target.value,
            redirect,
          })
        }}/>
        <input placeholder="New redirect" value={redirect} onChange={e => {
          setNewItem({
            domain,
            redirect: e.target.value,
          })
        }}/>
      </div>
      { isOpen
          ? <div className={css`display: inline-block;`}>
            <button onClick={() => cancel()}>Cancel</button>
            <button onClick={() => confirm()}>Confirm</button>
          </div>
        : <button onClick={() => {open()}}>Add site to filter (+)</button>
      }
    </div>
  );
}

const DeleteRow = ({
  domain, redirect, confirm, cancel
}) => {
  return <div tabIndex={0} className={rowClass}>
    <div className={cellClass}>{domain}</div>
    <div className={cellClass}>{redirect}</div>
    <CommitRowAction cancel={cancel} confirm={confirm}/>
  </div>
}

const CommitRowAction = ({
  cancel,
  confirm
}) => {
  return <div className={cx(cellClass, commitRowActionClass)}>
    <span className={commitRowActionCancelClass} tabIndex={0} onClick={() => cancel()}>Cancel</span>
    <span className={commitRowActionConfirmClass} tabIndex={0} onClick={() => confirm()}>Confirm</span>
  </div>
}

const Row = ({
  domain,
  redirect,
  updated,
  canShowControls,
  setEditingItem,
  setDeleteToConfirm,
}) => {

  return <div tabIndex={0} className={rowClass}>
    <div className={cellClass}>{domain}</div>
    <div className={cellClass}>{redirect}</div>
    <div className={cellClass}>{updated}</div>
    {canShowControls && <div className={cx(rowControlsClass, 'row-controls')}>
      <span tabIndex={0} className={startDeleteClass} onClick={() => setDeleteToConfirm(domain)}>Delete</span>
      <span tabIndex={0} onClick={() => setEditingItem({
        domain,
        newDomain: domain,
        redirect,
      })}>Edit</span>
    </div>}
  </div>
}

export default App;

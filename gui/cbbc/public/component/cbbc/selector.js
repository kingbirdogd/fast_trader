class Selector extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    this.counter = 0
    this.state.value = []
    this.state.options = []
    
    this.onChange = this.onChange.bind(this)
    this.getValue = this.getValue.bind(this)
    
    for(const val of this.props.config.defaultValue)
      this.state.options.push({value: this.getValue(val), label: val})
    for(const val of this.props.config.value)
      this.state.value.push({value: this.getValue(val), label: val, isFixed: true})
  }
  
  getValue = (val) => {
    var str = val+'-'+this.counter
    this.setState.counter = this.counter++
    return str
  }
  
  componentDidUpdate() {
    initTablePrice();
  }
  
  onChange(value, { action, removedValue }) {
    var g_obj = $.extend(true, {}, this.props.getStates())
    var l_obj = $.extend(true, {}, this.state)
    g_obj.config.value = value.map(item => Object.values(item)[1])
    l_obj.value = value
    if(action=='select-option') {
      var curVal = value.map(item => Object.values(item)[1])[value.length-1]
      l_obj.options.push({ value: this.getValue(curVal), label: curVal})
      l_obj.options.sort((a, b) => (a.label > b.label) ? -1 : 1)
    }
    else if (action=='remove-value') {
      l_obj.options = l_obj.options.filter(function( obj ) {
          return obj.value !== removedValue.value
      })
    }
    this.setState(l_obj)
    this.props.setStates({config: g_obj.config})
    // console.log(value, action, removedValue)
  }
  
  render() {
    const styles = {
      multiValue: (base, state) => {
        return state.data.isFixed ? { ...base, backgroundColor: 'gray' } : base;
      },
      multiValueLabel: (base, state) => {
        return state.data.isFixed
          ? { ...base, fontWeight: 'bold', color: 'white', paddingRight: 6 }
          : base;
      },
      multiValueRemove: (base, state) => {
        return state.data.isFixed ? { ...base, display: 'none' } : base;
      },
    }
    return (
      <div className='row'>
        <div className="col-12 col-sm-12 mb-2 mb-sm-2">
          <Select
            value={this.state.value}
            options={this.state.options}
            onChange={this.onChange}
            styles={styles}
            placeholder=""
            isClearable={false}
            isMulti
          />
        </div>
      </div>
    )
  }
}